#pragma once

#include <ctime>
#include <string>
#include <sstream>
#include <chrono>

namespace Construction {
    namespace Common {

        /**
            \class TimeDifference


         */
        template<int Base>
        class TimeDifference {
        public:
            TimeDifference(int value) : value(value*Base) { }
        public:
            int GetValue() const { return value; }
            void SetValue(int v) {
                value = v * Base;
            }
        public:
            operator TimeDifference<1>() const {
                return TimeDifference<1>(value);
            }
        private:
            int value;
        };

        typedef TimeDifference<1>           Seconds;
        typedef TimeDifference<60>          Minutes;
        typedef TimeDifference<3600>        Hours;
        typedef TimeDifference<86400>       Days;
        typedef TimeDifference<604800>      Weeks;
        typedef TimeDifference<31536000>    Years;

        /**
            \class Time

            Class representing time. It replaces the old timestamp class
            that employed the time_t structs and converted it internally
            on many occasions, which made it a mess to work with.

            The new class is based on the fact that the ranges for the
            single entries are known and a lot smaller than with an int.
            Storing hour, minute, second and milliseconds in four ints
            would mean 16 bytes for one single time entry, without any
            date information!
            This can be implemented a lot faster: We store everything in
            one single unsigned int. Milliseconds go from 0 to 1000, so
            we need 10 bytes, seconds and minutes range from 0 to 59
            so we need 6 bytes, respectively. Hours take values between 0
            and 23 which takes ridiculous 5 bytes. This gives a total
            of 27 bytes for one single timestamp.

            Comparison is easy since we can arrange the information such
            that the time order is correct: comparison then is just a
            question of mere integer comparison.

            The other advantage to the previous class is the fact that
            we can now use millisecond precision.
         */
        class Time {
        public:
            Time() : data(0) { }

            Time(int hour, int minute, int second, int millisecond = 0) : data(0) {
                SetHour(hour);
                SetMinute(minute);
                SetSecond(second);
                SetMillisecond(millisecond);
            }

            Time(const Time &other) : data(other.data) { }

            Time(Time &&other) : data(std::move(other.data)) { }

        public:
            Time &operator=(const Time &other) {
                data = other.data;
                return *this;
            }

            Time &operator=(Time &&other) {
                data = std::move(other.data);
                return *this;
            }

        public:
            inline int GetHour() const {
                return (data >> 22) & 31;
            }

            inline int GetMinute() const {
                return (data >> 16) & 63;
            }

            inline int GetSecond() const {
                return (data >> 10) & 63;
            }

            inline int GetMillisecond() const {
                return data & 1023;
                //return (data >> 17) & 1023;
            }

            inline void SetHour(int hour) {
                data |= (hour & 31) << 22;
            }

            inline void SetMinute(int minute) {
                data |= (minute & 63) << 16;
            }

            inline void SetSecond(int second) {
                data |= (second & 63) << 10;
            }

            inline void SetMillisecond(int ms) {
                data |= ms & 1023;
            }

            inline void SwapMilliseconds() {
                SetMillisecond(0);
            }
        public:
            static Time GetMinTime() { return Time(0,0,0,0); }
            static Time GetMaxTime() { return Time(23,59,59,999); }
        public:
            inline bool operator<(const Time &other) const {
                return data < other.data;
            }

            inline bool operator<=(const Time &other) const {
                return data <= other.data;
            }

            inline bool operator>(const Time &other) const {
                return data > other.data;
            }

            inline bool operator>=(const Time &other) const {
                return data >= other.data;
            }

            inline bool operator==(const Time &other) const {
                return data == other.data;
            }

            inline bool operator!=(const Time &other) const {
                return data != other.data;
            }

        public:
            friend std::ostream &operator<<(std::ostream &os, const Time &time) {
                os << time.ToString();
                return os;
            }

        private:
            // Padding helper
            std::string PaddedString(int c, char s, int length) const {
                std::string result = std::to_string(c);
                if (result.size() < length) {
                    result.insert(result.begin(), s);
                }
                return result;
            }

        public:
            std::string ToString(const std::string &format = "%T") const {
                std::string result = format;

                // Replace
                auto it = result.find("%T");
                if (it != std::string::npos) result.replace(it, 2, "%H:%M:%S");

                int hour = GetHour();
                int minute = GetMinute();
                int second = GetSecond();
                int ms = GetMillisecond();

                // Replace
                it = result.find("%H");
                if (it != std::string::npos) result.replace(it, 2, PaddedString(hour, '0', 2));

                it = result.find("%k");
                if (it != std::string::npos) result.replace(it, 2, PaddedString(hour, ' ', 2));

                it = result.find("%I");
                if (it != std::string::npos) result.replace(it, 2, PaddedString((hour % 12) + 1, '0', 2));

                it = result.find("%l");
                if (it != std::string::npos) result.replace(it, 2, PaddedString((hour % 12) + 1, ' ', 2));

                it = result.find("%P");
                if (it != std::string::npos) result.replace(it, 2, (hour < 12) ? "pm" : "am");

                it = result.find("%M");
                if (it != std::string::npos) result.replace(it, 2, PaddedString(minute, '0', 2));

                it = result.find("%S");
                if (it != std::string::npos) result.replace(it, 2, PaddedString(second, '0', 2));

                it = result.find("%L");
                if (it != std::string::npos) result.replace(it, 2, PaddedString(ms, '0', 2));

                return result;
            }

        public:
            static Time Now() {
                auto now = std::chrono::system_clock::now();
                auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
                auto fraction = now - seconds;

                time_t tt = std::chrono::system_clock::to_time_t(now);

                auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(fraction).count();

                struct tm *parts = std::localtime(&tt);
                Time result(parts->tm_hour, parts->tm_min, parts->tm_sec, milliseconds);

                return result;
            }
        public:
            friend Time operator+(const Time& time, const TimeDifference<1>& diff);
            friend Time operator+(const TimeDifference<1>& diff, const Time& time);
            friend Time operator-(const Time& time, const TimeDifference<1>& diff);
        private:
            unsigned int data;
        };

        /**
            \class Date

            Class representing a date. Similar to Time it stores the different
            fields inside one unsigned int. 11 bytes for the year since 1900,
            which gives the maximal year 3947, 4 bytes for a month and 5 for the
            day in the year. The rest is open so far and can later be extended
            to go to higher ranges.

            TODO: implement time zones
            TODO: implement parsing
         */
        class Date {
        public:
            enum Weekday {
                SUNDAY = 0,
                MONDAY = 1,
                TUESDAY = 2,
                WEDNESDAY = 3,
                THURSDAY = 4,
                FRIDAY = 5,
                SATURDAY = 6
            };

            enum Month {
                JANUARY = 1,
                FEBRUARY = 2,
                MARCH = 3,
                APRIL = 4,
                MAY = 5,
                JUNE = 6,
                JULY = 7,
                AUGUST = 8,
                SEPTEMBER = 9,
                OCTOBER = 10,
                NOVEMBER = 11,
                DECEMBER = 12
            };
        public:
            Date() : data(0) {
                SetYear(1970);
                SetMonth(JANUARY);
                SetDay(1);
            }

            Date(int year, Month month, int day) : data(0) {
                SetYear(year);
                SetMonth(month);
                SetDay(day);
            }

            Date(int year, int month, int day) : data(0) {
                SetYear(year);
                SetMonth(static_cast<Month>(month));
                SetDay(day);
            }

            Date(const Date &other) : data(other.data) { }

            Date(Date &&other) : data(std::move(other.data)) { }

        public:
            Date &operator=(const Date &other) {
                data = other.data;
                return *this;
            }

            Date &operator=(Date &&other) {
                data = std::move(other.data);
                return *this;
            }

        public:
            inline bool operator<(const Date &other) const {
                return data < other.data;
            }

            inline bool operator<=(const Date &other) const {
                return data <= other.data;
            }

            inline bool operator>(const Date &other) const {
                return data > other.data;
            }

            inline bool operator>=(const Date &other) const {
                return data >= other.data;
            }

            inline bool operator==(const Date &other) const {
                return data == other.data;
            }

            inline bool operator!=(const Date &other) const {
                return data != other.data;
            }

        public:
            inline int GetYear() const {
                return ((data >> 9) & 2047) + 1900;
            }

            inline Month GetMonth() const {
                return static_cast<Month>(((data >> 5) & 15) + 1);
            }

            inline int GetDay() const {
                return (data & 31) + 1;
            }

            inline void SetYear(int year) {
                data |= ((year - 1900) & 2047) << 9;
            }

            inline void SetMonth(Month month) {
                data |= ((static_cast<int>(month) - 1) & 15) << 5;
            }

            inline void SetDay(int day) {
                data |= day - 1 & 31;
            }
        public:
            inline Weekday GetDayOfWeek() const {
                int y = GetYear();
                int d = GetDay();
                int m = GetMonth();
                return static_cast<Weekday>((d += m < 3 ? y-- : y - 2, 23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400) %
                                            7);
            }

            inline bool IsWorkday() const {
                auto day = GetDayOfWeek();
                return day != SATURDAY && day != SUNDAY;
            }

            inline bool IsMonday() const { return GetDayOfWeek() == MONDAY; }

            inline bool IsTuesday() const { return GetDayOfWeek() == TUESDAY; }

            inline bool IsWednesday() const { return GetDayOfWeek() == WEDNESDAY; }

            inline bool IsThursday() const { return GetDayOfWeek() == THURSDAY; }

            inline bool IsFriday() const { return GetDayOfWeek() == FRIDAY; }

            inline bool IsSaturday() const { return GetDayOfWeek() == SATURDAY; }

            inline bool IsSunday() const { return GetDayOfWeek() == SUNDAY; }

            static std::string WeekdayToString(const Weekday &weekday) {
                switch (weekday) {
                    case SUNDAY:
                        return "Sunday";
                    case MONDAY:
                        return "Monday";
                    case TUESDAY:
                        return "Tuesday";
                    case WEDNESDAY:
                        return "Wednesday";
                    case THURSDAY:
                        return "Thursday";
                    case FRIDAY:
                        return "Friday";
                    case SATURDAY:
                        return "Saturday";
                }
            }

        public:
            bool IsLeapYear() const {
                int year = GetYear();
                return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);
            }

            static int DaysInMonth(int year, int month) {
                bool isLeap = (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);

                switch (month) {
                    case 1:
                        return 31;
                    case 2:
                        return (isLeap ? 29 : 28);
                    case 3:
                        return 31;
                    case 4:
                        return 30;
                    case 5:
                        return 31;
                    case 6:
                        return 30;
                    case 7:
                        return 31;
                    case 8:
                        return 31;
                    case 9:
                        return 30;
                    case 10:
                        return 31;
                    case 11:
                        return 30;
                    case 12:
                        return 31;
                }
                return -1;
            }

            static Date LastDayInMonth(const Date &date) {
                Date result = date;
                result.SetDay(DaysInMonth(date.GetYear(), date.GetMonth()));
                return result;
            }

            inline bool IsLastDayInMonth() const {
                return GetDay() == DaysInMonth(GetYear(), GetMonth());
            }

            int GetDayOfYear() const {
                int year = GetYear();
                int month = GetMonth();
                int day = GetDay();

                bool isLeap = ((year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0));
                int daysPerMonth[] = {31, (isLeap ? 29 : 28), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

                int result = 0;
                for (int m = 1; m < month; m++) {
                    result += daysPerMonth[m - 1];
                }
                return result + day;
            }
        public:
            Date GetNextDay() const {
                Date result = *this;

                result.SetDay(result.GetDay() + 1);
                if (result.GetDay() > DaysInMonth(result.GetYear(), result.GetMonth())) {
                    result.SetDay(1);
                    result.SetMonth(static_cast<Month>(result.GetMonth() + 1));
                }
                if (result.GetMonth() > 12) {
                    result.SetMonth(JANUARY);
                    result.SetYear(result.GetYear() + 1);
                }

                return result;
            }
        public:
            static Date GetMinDate() { return Date(1900,1,1); }
            static Date GetMaxDate() { return Date(3947,12,31); }
        public:
            friend std::ostream &operator<<(std::ostream &os, const Date &date) {
                os << date.ToString();
                return os;
            }

        private:
            // Padding helper
            static std::string PaddedString(int c, char s, int length) {
                std::string result = std::to_string(c);
                if (result.size() < length) {
                    result.insert(result.begin(), s);
                }
                return result;
            }
        public:
            std::string ToString(const std::string &format = "%F") const {
                std::string result = format;

                // Default strings
                auto it = result.find("%D");
                if (it != std::string::npos) result.replace(it, 2, "%m/%d/%y");

                it = result.find("%x");
                if (it != std::string::npos) result.replace(it, 2, "%m/%d/%y");

                it = result.find("%F");
                if (it != std::string::npos) result.replace(it, 2, "%Y-%m-%d");

                // Years
                int Y = GetYear();
                int C = Y / 100;
                int y = Y % 100;

                // Months
                int m = GetMonth();
                std::string B;
                switch (m) {
                    case 1:
                        B = "January";
                        break;
                    case 2:
                        B = "February";
                        break;
                    case 3:
                        B = "March";
                        break;
                    case 4:
                        B = "April";
                        break;
                    case 5:
                        B = "May";
                        break;
                    case 6:
                        B = "June";
                        break;
                    case 7:
                        B = "July";
                        break;
                    case 8:
                        B = "August";
                        break;
                    case 9:
                        B = "September";
                        break;
                    case 10:
                        B = "October";
                        break;
                    case 11:
                        B = "November";
                        break;
                    case 12:
                        B = "December";
                        break;
                }
                std::string b = B.substr(0, 3);

                int d = GetDay();
                int j = GetDayOfYear();

                // Week day
                int w = GetDayOfWeek();
                std::string A = WeekdayToString(GetDayOfWeek());
                std::string a = A.substr(0, 3);

                // Replace
                it = result.find("%Y");
                if (it != std::string::npos) result.replace(it, 2, PaddedString(Y, '0', 4));

                it = result.find("%C");
                if (it != std::string::npos) result.replace(it, 2, PaddedString(C, '0', 2));

                it = result.find("%y");
                if (it != std::string::npos) result.replace(it, 2, PaddedString(y, '0', 2));

                it = result.find("%m");
                if (it != std::string::npos) result.replace(it, 2, PaddedString(m, '0', 2));

                it = result.find("%B");
                if (it != std::string::npos) result.replace(it, 2, B);

                it = result.find("%b");
                if (it != std::string::npos) result.replace(it, 2, b);

                it = result.find("%h");
                if (it != std::string::npos) result.replace(it, 2, b);

                it = result.find("%d");
                if (it != std::string::npos) result.replace(it, 2, PaddedString(d, '0', 2));

                it = result.find("%e");
                if (it != std::string::npos) result.replace(it, 2, PaddedString(d, ' ', 4));

                it = result.find("%j");
                if (it != std::string::npos) result.replace(it, 2, PaddedString(j, '0', 3));

                it = result.find("%A");
                if (it != std::string::npos) result.replace(it, 2, A);

                it = result.find("%a");
                if (it != std::string::npos) result.replace(it, 2, a);

                it = result.find("%u");
                if (it != std::string::npos) result.replace(it, 2, std::to_string(w + 1));

                it = result.find("%w");
                if (it != std::string::npos) result.replace(it, 2, std::to_string(w));


                it = result.find("%n");
                if (it != std::string::npos) result.replace(it, 2, "\n");
                it = result.find("%t");
                if (it != std::string::npos) result.replace(it, 2, "\t");

                return result;
            }

        public:
            static Date Today() {
                auto now = std::chrono::system_clock::now();
                std::time_t now_c = std::chrono::system_clock::to_time_t(now);
                struct tm *parts = std::localtime(&now_c);

                Date result(1900 + parts->tm_year, static_cast<Month>(1 + parts->tm_mon), parts->tm_mday);

                return result;
            }
        public:
            friend Date operator+(const Date& time, const TimeDifference<1>& diff);
            friend Date operator+(const TimeDifference<1>& diff, const Date& time);
            friend Date operator-(const Date& time, const TimeDifference<1>& diff);
        private:
            unsigned int data;
        };

        /**
            \class Datetime
         */
        class Datetime {
        public:
            Datetime() : Datetime(Datetime::Now()) { }

            Datetime(int year, int month, int day, int hour, int min, int sec, int millisecond=0) : date(year, month, day), time(hour, min, sec, millisecond) { }

            Datetime(const Date& date) : date(date) { }
            Datetime(Date&& date) : date(std::move(date)) { }

            Datetime(const Time& time) : time(time) { }
            Datetime(Time&& time) : time(std::move(time)) { }

            Datetime(const Date& date, const Time& time) : date(date), time(time) { }
            Datetime(Date&& date, Time&& time) : date(std::move(date)), time(std::move(time)) { }

            Datetime(const Datetime& other) : date(other.date), time(other.time) { }
            Datetime(Datetime&& other) : date(std::move(other.date)), time(std::move(other.time)) { }

            Datetime(const std::string& str) : Datetime(Datetime::FromString(str)) { }
        public:
            Datetime& operator=(const Datetime& other) {
                date = other.date;
                time = other.time;
                return *this;
            }

            Datetime& operator=(Datetime&& other) {
                date = std::move(other.date);
                time = std::move(other.time);
                return *this;
            }

            Datetime& operator=(const Date& other) {
                date = other;
                return *this;
            }

            Datetime& operator=(Date&& other) {
                date = std::move(other);
                return *this;
            }

            Datetime& operator=(const Time& other) {
                time = other;
                return *this;
            }

            Datetime& operator=(Time&& other) {
                time = std::move(other);
                return *this;
            }
        public:
            inline bool operator==(const Datetime& other) const {
                return date == other.date && time == other.time;
            }

            inline bool operator!=(const Datetime& other) const {
                return date != other.date || time != other.time;
            }

            inline bool operator<(const Datetime& other) const {
                if (date < other.date) return true;
                else if (date > other.date) return false;
                return time < other.time;
            }

            inline bool operator<=(const Datetime& other) const {
                if (date < other.date) return true;
                else if (date > other.date) return false;
                return time <= other.time;
            }

            inline bool operator>(const Datetime& other) const {
                if (date < other.date) return false;
                else if (date > other.date) return true;
                return time > other.time;
            }

            inline bool operator>=(const Datetime& other) const {
                if (date < other.date) return false;
                else if (date > other.date) return true;
                return time >= other.time;
            }
        public:
            Time GetTime() const { return time; }
            Time& GetTime() { return time; }

            Date GetDate() const { return date; }
            Date& GetDate() { return date; }
        public:
            static Datetime GetMinDate() { return Datetime(Date::GetMinDate(), Time::GetMinTime()); }
            static Datetime GetMaxDate() { return Datetime(Date::GetMaxDate(), Time::GetMaxTime()); }
        public:
            inline int GetDay() const { return date.GetDay(); }
            inline int GetMonth() const { return date.GetMonth(); }
            inline int GetYear() const { return date.GetYear(); }

            inline int GetHour() const { return time.GetHour(); }
            inline int GetMinute() const { return time.GetMinute(); }
            inline int GetSecond() const { return time.GetSecond(); }
            inline int GetMillisecond() const { return time.GetMillisecond(); }

            inline void SetDay(int day) { date.SetDay(day); }
            inline void SetMonth(Date::Month month) { date.SetMonth(month); }
            inline void SetYear(int year) { date.SetYear(year); }

            inline void SetHour(int hour) { time.SetHour(hour); }
            inline void SetMinute(int minute) { time.SetMinute(minute); }
            inline void SetSecond(int second) { time.SetSecond(second); }
            inline void SetMillisecond(int ms) { time.SetMillisecond(ms); }

            inline void SwapMilliseconds() { time.SwapMilliseconds(); }
        public:
            inline Date::Weekday GetDayOfWeek() const { return date.GetDayOfWeek(); }
            inline int GetDayOfYear() const { return date.GetDayOfYear(); }

            inline bool IsWorkday() const { return date.IsWorkday(); }
            inline bool IsMonday() const { return date.IsMonday(); }
            inline bool IsTuesday() const { return date.IsTuesday(); }
            inline bool IsWednesday() const { return date.IsWednesday(); }
            inline bool IsThursday() const { return date.IsThursday(); }
            inline bool IsFriday() const { return date.IsFriday(); }
            inline bool IsSaturday() const { return date.IsSaturday(); }
            inline bool IsSunday() const { return date.IsSunday(); }

            inline bool IsLeapYear() const { return date.IsLeapYear(); }
            inline static int DaysInMonth(int year, int month) { return Date::DaysInMonth(year, month); }
            inline static Date LastDayInMonth(const Datetime& datetime) { return Date::LastDayInMonth(datetime.date); }
            inline bool IsLastDayInMonth() const { return date.IsLastDayInMonth(); }
        public:
            int GetSecondsSinceEpoch() const {
                int daysSinceEpoch = date.GetDayOfYear()-1;
                for (int y=1970; y<date.GetYear(); y++) {
                    if ((y % 4) == 0 && ((y % 100) != 0 || (y % 400) == 0)) {
                        daysSinceEpoch += 366;
                    } else daysSinceEpoch += 365;
                }

                // TODO: include timezone

                return daysSinceEpoch * 86400 + 3600*time.GetHour() + 60*time.GetMinute() + time.GetSecond();
            }

            static Datetime FromSecondsSinceEpoch(int seconds) {
                int daysSinceEpoch = seconds / 86400;
                int secondsSinceEpoch = seconds % 86400;

                // Calculate year
                int year=1970;
                while (true) {
                    bool isLeap = (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);

                    if (isLeap) {
                        if (daysSinceEpoch <= 366) break;
                        daysSinceEpoch -= 366;
                    } else {
                        if (daysSinceEpoch <= 365) break;
                        daysSinceEpoch -= 365;
                    }

                    year++;
                }

                // Calculate month
                int month = 1;
                while (true) {
                    int daysInMonth = Date::DaysInMonth(year, month);
                    if (daysSinceEpoch <= daysInMonth) break;
                    daysSinceEpoch -= daysInMonth;
                    month++;
                }

                // Calculate hour
                int hour=0;
                while (secondsSinceEpoch > 3600) {
                    secondsSinceEpoch -= 3600;
                    hour++;
                }

                // Calculate minute
                int minute=0;
                while (secondsSinceEpoch > 60) {
                    secondsSinceEpoch -= 60;
                    minute++;
                }

                return Datetime(year, month, daysSinceEpoch+1, hour, minute, secondsSinceEpoch);
            }
        public:
            friend std::ostream& operator<<(std::ostream& os, const Datetime& time) {
                os << time.ToString();
                return os;
            }

            std::string ToString(const std::string& format="%F %T") const {
                std::string result = format;

                // Do some magic like timezones etc.

                result = date.ToString(result);
                result = time.ToString(result);

                return result;
            }
        public:
            static Datetime Now() {
                auto now = std::chrono::system_clock::now();
                auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
                auto fraction = now - seconds;

                time_t tt = std::chrono::system_clock::to_time_t(now);

                auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(fraction).count();

                struct tm *parts = std::localtime(&tt);
                Datetime result(parts->tm_year + 1900, parts->tm_mon+1, parts->tm_mday, parts->tm_hour, parts->tm_min, parts->tm_sec, milliseconds);

                return result;
            }
        public:
            static Datetime FromString(const std::string& str) {
                std::stringstream ss(str);

                int year, month, day, hour, minute, second;
                std::string timezone;
                char trash;

                // Format: yy-mm-dd hh:MM:ss TIMEZONE
                ss >> year; ss >> trash;
                ss >> month; ss >> trash;
                ss >> day;

                ss >> hour; ss >> trash;
                ss >> minute; ss >> trash;
                ss >> second; ss >> timezone;

                // Convert timezone to UTC
                //double tz = -TimeStamp::GetTimezoneFromName(timezone);
                //hour    += static_cast<int>(tz);
                //minute  += round(fmod(tz, 1)*60);

                // Set the data from the timestamp
                Datetime result (year, month, day, hour, minute, second);
                //m_timezone = static_cast<int>(tz);

                return result;
            }
        public:
            friend Datetime operator+(const Datetime& time, const TimeDifference<1>& diff);
            friend Datetime operator+(const TimeDifference<1>& diff, const Datetime& time);
            friend Datetime operator-(const Datetime& time, const TimeDifference<1>& diff);

            /**
                Calculates the difference between two dates
             */
            inline double operator-(const Datetime& other) {
                return GetSecondsSinceEpoch() - other.GetSecondsSinceEpoch() + (GetMillisecond()-other.GetMillisecond())/1000;
            }
        private:
            Date date;
            Time time;
        };

        /**
            Operators
         */
        inline Datetime operator+(const Datetime& time, const TimeDifference<1>& diff) {
            auto result = Datetime::FromSecondsSinceEpoch(time.GetSecondsSinceEpoch() + diff.GetValue());
            result.SetMillisecond(time.GetMillisecond());
            return result;
        }

        inline Datetime operator+(const TimeDifference<1>& diff, const Datetime& time) {
            auto result = Datetime::FromSecondsSinceEpoch(time.GetSecondsSinceEpoch() + diff.GetValue());
            result.SetMillisecond(time.GetMillisecond());
            return result;
        }

        inline Datetime operator-(const Datetime& time, const TimeDifference<1>& diff) {
            auto result = Datetime::FromSecondsSinceEpoch(time.GetSecondsSinceEpoch() - diff.GetValue());
            result.SetMillisecond(time.GetMillisecond());
            return result;
        }

        inline Time operator+(const Time& time, const TimeDifference<1>& diff) {
            return (Datetime(time) + diff).GetTime();
        }

        inline Time operator+(const TimeDifference<1>& diff, const Time& time) {
            return (Datetime(time) + diff).GetTime();
        }

        inline Time operator-(const Time& time, const TimeDifference<1>& diff) {
            return (Datetime(time) - diff).GetTime();
        }

        inline Date operator+(const Date& time, const TimeDifference<1>& diff) {
            return (Datetime(time) + diff).GetDate();
        }

        inline Date operator+(const TimeDifference<1>& diff, const Date& time) {
            return (Datetime(time) + diff).GetDate();
        }

        inline Date operator-(const Date& time, const TimeDifference<1>& diff) {
            return (Datetime(time) - diff).GetDate();
        }

    }
}

inline Construction::Common::Seconds operator"" _s (unsigned long long s) { return Construction::Common::Seconds(s); }
inline Construction::Common::Minutes operator"" _min (unsigned long long min) { return Construction::Common::Minutes(min); }
inline Construction::Common::Hours operator"" _h (unsigned long long h) { return Construction::Common::Hours(h); }
inline Construction::Common::Days operator"" _d (unsigned long long d) { return Construction::Common::Days(d); }
inline Construction::Common::Weeks operator"" _w (unsigned long long w) { return Construction::Common::Weeks(w); }
inline Construction::Common::Years operator"" _y (unsigned long long y) { return Construction::Common::Years(y); }
