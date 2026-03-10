#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Student
{
    int regNo;
    char name[50];
    int totalClasses;
    int attendedClasses;
};

void addStudent()
{
    FILE *fp;
    struct Student s;

    fp = fopen("attendance.txt", "a");

    if(fp == NULL)
    {
        printf("File Error\n");
        return;
    }

    printf("Enter Register Number: ");
    scanf("%d", &s.regNo);

    printf("Enter Name: ");
    scanf("%s", s.name);

    s.totalClasses = 0;
    s.attendedClasses = 0;

    fprintf(fp, "%d %s %d %d\n",
            s.regNo,
            s.name,
            s.totalClasses,
            s.attendedClasses);

    fclose(fp);

    printf("Student Added Successfully\n");
}

void markAttendance()
{
    FILE *fp, *temp;
    struct Student s;
    int reg;
    char status;

    fp = fopen("attendance.txt", "r");
    temp = fopen("temp.txt", "w");

    if(fp == NULL || temp == NULL)
    {
        printf("File Error\n");
        return;
    }

    printf("Enter Register Number: ");
    scanf("%d", &reg);

    printf("Present or Absent (P/A): ");
    scanf(" %c", &status);

    while(fscanf(fp,"%d %s %d %d",
                &s.regNo,
                s.name,
                &s.totalClasses,
                &s.attendedClasses) == 4)
    {
        if(s.regNo == reg)
        {
            s.totalClasses++;

            if(status == 'P' || status == 'p')
                s.attendedClasses++;
        }

        fprintf(temp,"%d %s %d %d\n",
                s.regNo,
                s.name,
                s.totalClasses,
                s.attendedClasses);
    }

    fclose(fp);
    fclose(temp);

    remove("attendance.txt");
    rename("temp.txt","attendance.txt");

    printf("Attendance Updated Successfully\n");
}

void displayStudents()
{
    FILE *fp;
    struct Student s;

    fp = fopen("attendance.txt","r");

    if(fp == NULL)
    {
        printf("File Error\n");
        return;
    }

    printf("\nRegNo   Name   TotalClasses   AttendedClasses\n");

    while(fscanf(fp,"%d %s %d %d",
                &s.regNo,
                s.name,
                &s.totalClasses,
                &s.attendedClasses) == 4)
    {
        printf("%d %s %d %d\n",
               s.regNo,
               s.name,
               s.totalClasses,
               s.attendedClasses);
    }

    fclose(fp);
}

void shortageList()
{
    FILE *fp;
    struct Student s;
    float percent;

    fp = fopen("attendance.txt","r");

    if(fp == NULL)
    {
        printf("File Error\n");
        return;
    }

    printf("\nStudents Below 75%% Attendance\n");

    while(fscanf(fp,"%d %s %d %d",
                &s.regNo,
                s.name,
                &s.totalClasses,
                &s.attendedClasses) == 4)
    {
        if(s.totalClasses > 0)
        {
            percent = (float)s.attendedClasses / s.totalClasses * 100;

            if(percent < 75)
            {
                printf("%d %s %.2f%%\n",
                       s.regNo,
                       s.name,
                       percent);
            }
        }
    }

    fclose(fp);
}

void classReport()
{
    FILE *fp;
    struct Student s;
    float percent;
    float total = 0;
    int count = 0;

    fp = fopen("attendance.txt","r");

    if(fp == NULL)
    {
        printf("File Error\n");
        return;
    }

    while(fscanf(fp,"%d %s %d %d",
                &s.regNo,
                s.name,
                &s.totalClasses,
                &s.attendedClasses) == 4)
    {
        if(s.totalClasses > 0)
        {
            percent = (float)s.attendedClasses / s.totalClasses * 100;
            total += percent;
            count++;
        }
    }

    if(count > 0)
        printf("Class Average Attendance: %.2f%%\n", total/count);
    else
        printf("No attendance data available\n");

    fclose(fp);
}

int main()
{
    int choice;

    while(1)
    {
        printf("\nAttendance Manager\n");
        printf("1 Add Student\n");
        printf("2 Mark Attendance\n");
        printf("3 Display Students\n");
        printf("4 Students Below 75%%\n");
        printf("5 Class Report\n");
        printf("6 Exit\n");

        printf("Enter Choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                addStudent();
                break;

            case 2:
                markAttendance();
                break;

            case 3:
                displayStudents();
                break;

            case 4:
                shortageList();
                break;

            case 5:
                classReport();
                break;

            case 6:
                exit(0);

            default:
                printf("Invalid Choice\n");
        }
    }

    return 0;
}
