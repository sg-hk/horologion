#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

#define HRLG "/.local/share/horologion/"

/*horologion bell*/

double to_rad (double deg) {return deg * (M_PI / 180.0);}
double to_deg (double rad) {return rad * (180.0 / M_PI);}
double norm_deg (double deg)
{
        while (deg < 0)
                deg += 360;
        while (deg >= 360)
                deg -= 360;
        return deg;
}
double norm_hours (double hours)
{
        while (hours < 0)
                hours += 24;
        while (hours >= 24)
                hours -= 24;
        return hours;
}
double get_zenith (double altitude) 
{
        const double OFFICIAL_ZENITH = 90.8333;
        const double EARTH_RADIUS = 6371000.0; // meters
        double dip = to_deg(acos(EARTH_RADIUS / (EARTH_RADIUS + altitude)));
        return OFFICIAL_ZENITH - dip;
}

/* algorithm: https://edwilliams.org/sunrise_sunset_algorithm.htm */
double sun_event (double latitude, double longitude, double altitude, 
                  bool want_sunrise)
{
        time_t now = time(NULL);
        int N = localtime(&now)->tm_yday + 1; // day of the year
        int local_offset = localtime(&now)->tm_gmtoff / 3600; // UTC+...
        double lng_hour = longitude / 15.0;

        /* get approximate time t */
        double t = want_sunrise ? N + ((6.0 - lng_hour) / 24.0) :
                                  N + ((18.0 - lng_hour) / 24.0);

        /* long series of calculations to refine t */
        double M = (0.9856 * t) - 3.289; // sun's mean anomaly
        double L = M + (1.916 * sin(to_rad(M))) 
                 + (0.020 * sin(to_rad(2 * M))) + 282.634;
        L = norm_deg(L); // sun's true longitude
        double RA = to_deg(atan(0.91764 * tan(to_rad(L))));
        RA = norm_deg(RA); // sun's right ascension
        double Lquadrant = floor(L / 90.0) * 90.0;
        double RAquadrant = floor(RA / 90.0) * 90.0;
        RA = RA + (Lquadrant - RAquadrant); // adjusted RA
        RA /= 15.0; // in hours
        double sin_dec = 0.39782 * sin(to_rad(L));
        double dec = asin(sin_dec); // sun's declination
        double cos_dec = cos(dec);
        double zenith = get_zenith(altitude);
        double cosH = (cos(to_rad(zenith)) - (sin_dec * sin(to_rad(latitude))))
                / (cos_dec * cos(to_rad(latitude))); // local hour angle
        if (cosH > 1.0 && cosH < -1.0) return NAN; // sun never rises/sets
        double H = want_sunrise ? 360.0 - to_deg(acos(cosH)) :
                                  to_deg(acos(cosH));
        H /= 15.0;

        /* finally, get the actual time T */
        double T = H + RA - (0.06571 * t) - 6.622;
        double UT = T - lng_hour;
        UT = norm_hours(UT);
        double localT = norm_hours(UT + local_offset);

        return localT;
}


volatile sig_atomic_t stop_flag = 0;
void handle_signal(int signal) 
{
        (void)signal; // suppress compiler warning
        stop_flag = 1;
}

int main(void)
{
        int sunrise = sun_event(latitude, longitude, altitude, true) * 60;
        int sunset = sun_event(latitude, longitude, altitude, false) * 60;
        int cn_hour = (sunset - sunrise) / 12; // canonical hour

        const int sch_arr[8] = {
                sunrise - 20,
                sunrise,
                sunrise + cn_hour * 3,
                sunrise + cn_hour * 6,
                sunrise + cn_hour * 9,
                sunset,
                sunset + cn_hour * 2,
                sunset + cn_hour * 4
        };

        const char *name[8] = {
                "matins", 
                "first", 
                "third", 
                "sixth",
                "ninth", 
                "vespers", 
                "compline", 
                "nocturns"
        };

        char path[125];
        snprintf(path, sizeof(path), "%s%s", getenv("HOME"), HRLG);
        char logpath[128];
        snprintf(logpath, sizeof(logpath), "%s%s", path, "log");

        /* set up signal handlers for SIGINT (Ctrl+C) and SIGTERM (kill) */
        struct sigaction sa;
        sa.sa_handler = handle_signal;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGTERM, &sa, NULL);

        while (!stop_flag) {
                time_t now = time(NULL);
                struct tm *t = localtime(&now);
                int now_m = t->tm_hour * 60 + t->tm_min;
                int index = 0, until = 0;

                /* pick the next prayer
                 * note: will skip prayer if now is exactly prayer time */
                for (int i = 0; i < 8; ++i) {
                        if (now_m >= sch_arr[i])
                                continue;
                        index = i;
                        break;
                }

                /* compute time until next prayer and log it
                 * this overwrites the file every time, to avoid an infinite log */
                until = sch_arr[index] - now_m;
                int hours = until / 60;
                int minutes = until % 60; 
                FILE *log = fopen(logpath, "w");
                if (!log) {
                        perror("can't access log");
                        exit(1);
                }
                fprintf(log, "%s in %02d:%02d\n", name[index], hours, minutes);
                fclose(log);

                /* notify user one minute ahead:
                 * - left click to dismiss
                 * - right click to display prayer */
                char cmd[256];
                if (until == 1) {
                        snprintf(cmd, sizeof(cmd),
                                "herbe '%s prayer in one minute'"
                                "&& %s -e sh -c 'cat %s%s | less'",
                                name[index], terminal, path, name[index]
                                );
                        printf("\a\n"); // bell sound on systems that have it

                        pid_t pid = fork(); 
                        if (pid == -1) {
                                perror("fork");
                        } else if (pid == 0) {
                                execl("/bin/sh", "sh", "-c", cmd, (char*)NULL);
                                perror("execl");
                                exit(1);
                        }
                }

                /* sleep until next minute */
                int secs = t->tm_sec;
                int sleep_for = 60 - secs;
                sleep(sleep_for);
        }

        return 0;
}
