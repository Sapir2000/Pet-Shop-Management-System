#pragma once

struct tm* time_inf();
void log_action(const char* description);
char* organisde_current_time();
char* organisde_current_date();
double calculate_days_passed(char* date);