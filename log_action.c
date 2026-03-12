#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "log_action.h"

struct tm* time_inf()
{
    time_t raw_time;
    struct tm* time_info;

    time(&raw_time); // קבלת הזמן הנוכחי בשניות
    time_info = localtime(&raw_time); // המרה לזמן מקומי (שנה, חודש, שעה וכו')
    return time_info;
}

void log_action(const char* description)
{
    char* organisde_time= organisde_current_time();

    FILE* file = fopen("activity_log.txt", "a");
    if (!file)
    {
        printf("Error, log_action file failed opening");
        return;
    }
    fprintf(file, "[%s] %s\n", organisde_time, description);
    fclose(file);

}

char* organisde_current_time()
{
    static char organisde_time[80];
    struct tm* time_info = time_inf();

    strftime(organisde_time, sizeof(organisde_time), "%d/%m/%Y %H:%M:%S", time_info);
    return organisde_time;
}

char* organisde_current_date()
{
    static char organisde_time[80];
    struct tm* time_info = time_inf();

    strftime(organisde_time, sizeof(organisde_time), "%d/%m/%Y", time_info);
    return organisde_time;
}

double calculate_days_passed(char* date)
{
	int day, month, year;
	struct tm bought_time = {0};
	time_t t_bought, t_now;
	double seconds_diff;

    sscanf(date, "%d/%d/%d", &day, &month, &year);

	bought_time.tm_year = year - 1900; // שנים נספרות מ-1900
	bought_time.tm_mon = month - 1;    // חודשים נספרים מ-0 עד 11
	bought_time.tm_mday = day;

	t_bought = mktime(&bought_time); // המרה -time_t (שניות מאז 1970)
	time(&t_now);

	seconds_diff = difftime(t_now, t_bought);
	return seconds_diff / (60 * 60 * 24); // חלוקה במספר השניות ביממה
}