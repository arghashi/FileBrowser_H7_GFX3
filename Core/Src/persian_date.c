/*
 * persian_date.c
 *
 *  Created on: May 13, 2025
 *      Author: Nima
 */

#include "persian_date.h"


PersianDate GregorianToPersian(int gYear, int gMonth, int gDay) {
    int jd, year, month, day;
    // محاسبه Julian Day
    jd = (1461 * (gYear + 4800 + (gMonth - 14)/12))/4
        + (367 * (gMonth - 2 - 12 * ((gMonth - 14)/12)))/12
        - (3 * ((gYear + 4900 + (gMonth - 14)/12)/100))/4
        + gDay - 32075;

    // تبدیل به تاریخ شمسی
    jd -= 1948320; // مبدأ تقویم شمسی (۱ فروردین ۱۳۴۸)

    year = 33 * jd / 12053;
    jd = jd % 12053;
    year += 1348 + (4 * jd)/1461;
    jd = (4 * jd) % 1461;

    month = (jd < 1530) ? 1 + jd/306 : 7 + (jd-1530)/306;
    day = (jd % 306) % 31 + 1;

    PersianDate result;
    result.year = year;
    result.month = month;
    result.day = day;
    return result;
}
