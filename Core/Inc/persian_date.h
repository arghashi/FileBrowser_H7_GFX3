/*
 * persian_date.h
 *
 *  Created on: May 13, 2025
 *      Author: Nima
 */

#ifndef INC_PERSIAN_DATE_H_
#define INC_PERSIAN_DATE_H_


typedef struct {
    int year;
    int month;
    int day;
} PersianDate;

PersianDate GregorianToPersian(int gYear, int gMonth, int gDay);


#endif /* INC_PERSIAN_DATE_H_ */
