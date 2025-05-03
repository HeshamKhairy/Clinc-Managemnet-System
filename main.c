#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "STD_TYPES.h"
#define ADMIN_MODE 1
#define USER_MODE 2
#define EXIT_PROGRAM 3
#define MAX_PATIENTS 100

typedef struct
{
    u8 name[40];
    u8 age;
    u8 gender;
    u64 ID;
} Patient;

typedef struct 
{
    char time[30];    
    u8 reserved;
    u64 patientID;
} Slot;

Slot slots[5];
Patient patients[MAX_PATIENTS];
u8 patientCount = 0;

// Function declarations

u8 Admin(void);
void User(void);
void ViewPatientRecord(void);
void ViewTodaysReservations(void);
u8 IsPasswordCorrect(void);
void AddNewPatient(void);
void ScanValidatedName(u8 *str);
u8 IsNameValid(const u8 *name);
u8 IsIDExists(u64 id);
s8 FindPatientIndex(u64 id);
void EditPatientRecord(void);
void ReserveSlotWithDoctor(void);
void initializeSlots(Slot slots[], u8 size);
u8 HasExistingReservation(u64 id);
void CancelReservation(void);


void ClearInputBuffer()
{
    u32 c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main()
{
    u8 userType;
    u8 exitFlag = 0;
    u8 inputBuffer[20];
    u8 tempChar;
    
    initializeSlots(slots, 5);

    printf("Welcome to H-Clinic\n");

    while (!exitFlag)
    {
        printf("\n============================\n");
        printf("Choose a mode:\n");
        printf("1. Admin\n");
        printf("2. User\n");
        printf("3. Exit\n");
        printf("Select an option: ");

        fgets(inputBuffer, sizeof(inputBuffer), stdin);
        if (sscanf(inputBuffer, "%hhu %c", &userType, &tempChar) != 1)
        {
            userType = 0;
        }

        switch (userType)
        {
        case ADMIN_MODE:
            if (IsPasswordCorrect())
            {
                while (Admin() == 0);
            }
            else
            {
                exitFlag = 1;
            }
            break;

        case USER_MODE:
            User();
            break;

        case EXIT_PROGRAM:
            printf("Thank you for using H-Clinic. Goodbye!\n");
            exitFlag = 1;
            break;

        default:
            printf("Invalid option! please select a valid option.\n");
            break;
        }
    }

    return 0;
}

u8 Admin(void)
{
    u8 option;
    u8 exitAdmin = 0;
    u8 inputBuffer[20];
    u8 tempChar;
    
    printf("\n=== Admin Menu ===\n");
    printf("1. Add new patient record\n");
    printf("2. Edit patient record\n");
    printf("3. Reserve a slot with the doctor\n");
    printf("4. Cancel reservation.\n");
    printf("5. Exit to Main Menu\n");
    printf("Please select an option: ");

    fgets(inputBuffer, sizeof(inputBuffer), stdin);
    if (sscanf(inputBuffer, "%hhu %c", &option, &tempChar) != 1)
    {
        option = 0;
    }

    switch (option)
    {
    case 1:
        AddNewPatient();
        break;

    case 2:
        EditPatientRecord();
        break;

    case 3:
        ReserveSlotWithDoctor();
        break;

    case 4:
        CancelReservation();
        break;

    case 5:
        printf("Returning to Main Menu...\n");
        exitAdmin = 1;
        break;

    default:
        printf("Invalid option! Please select a valid option.\n");
        break;
    }

    return exitAdmin;
}

void User(void)
{
    u8 option;
    u8 inputBuffer[20];
    u8 tempChar;
    u8 keepRunning = 1;

    while (keepRunning)
    {
        printf("\n=== User Menu ===\n");
        printf("1. View patient record\n");
        printf("2. View reservations for today\n");
        printf("3. Exit to Main Menu\n");
        printf("Please select an option: ");

        fgets(inputBuffer, sizeof(inputBuffer), stdin);
        if (sscanf(inputBuffer, "%hhu %c", &option, &tempChar) != 1)
        {
            option = 0;
        }

        switch (option)
        {
        case 1:
            ViewPatientRecord();
            break;
        case 2:
            ViewTodaysReservations();
            break;
        case 3:
            printf("Returning to Main Menu...\n");
            keepRunning = 0;
            break;
        default:
            printf("Invalid option! Please select a valid option.\n");
            break;
        }
    }
}


void ViewPatientRecord(void)
{
    u8 inputBuffer[100];
    u64 id;
    s8 index;
    u32 valid = 1;

    printf("Enter Patient ID to view: ");
    fgets(inputBuffer, sizeof(inputBuffer), stdin);
    
    size_t len = strlen(inputBuffer);
    if (len > 0 && inputBuffer[len - 1] == '\n')
    {
        inputBuffer[len - 1] = '\0';
        len--;
    }

    if (len != 14)
    {
        printf("Invalid ID! ID must be 14 digits.\n");
        return;
    }

    
    for (size_t i = 0; (i < len) && (valid != 0); i++)
    {
        if (inputBuffer[i] < '0' || inputBuffer[i] > '9')
        {
            valid = 0;
        }
    }

    if (!valid)
    {
        printf("Invalid ID! ID must contain digits only.\n");
        return;
    }

    sscanf(inputBuffer, "%llu", &id);

    index = FindPatientIndex(id);
    if (index == -1)
    {
        printf("No patient found with the entered ID.\n");
        return;
    }

    printf("\n--- Patient Record ---\n");
    printf("Name   : %s\n", patients[index].name);
    printf("Age    : %hhu\n", patients[index].age);
    printf("Gender : %c\n", patients[index].gender);
    printf("ID     : %llu\n", patients[index].ID);
    printf("----------------------\n");
}

void ViewTodaysReservations(void)
{
    printf("\n--- Today's Reservations ---\n");
    u8 found = 0;
    for (int i = 0; i < 5; i++)
    {
        if (slots[i].reserved)
        {
            printf("Slot %d: %s --> Patient ID: %llu\n", i + 1, slots[i].time, slots[i].patientID);
            found = 1;
        }
    }

    if (!found)
    {
        printf("No reservations for today.\n");
    }

    printf("-----------------------------\n");
}

u8 IsPasswordCorrect(void)
{
    u8 flag = 0;
    u8 buffer[20];
    u32 password;
    u8 attemptCount = 0;

    while (attemptCount < 3)
    {
        printf("Please enter your Password: ");
        fgets(buffer, sizeof(buffer), stdin);

        if (sscanf(buffer, "%u", &password) != 1)
        {
            printf("Invalid input! Please enter digits only.\n");
        }
        else
        {
            if (password == 1234)
            {
                flag = 1;
                break;
            }
            else
            {
                printf("Wrong Password, Please try again.\n");
            }
        }

        attemptCount++;

        if (attemptCount == 3)
        {
            printf("Wrong Password 3 times, Your account is locked\n");
        }
    }

    return flag;
}


void AddNewPatient(void)
{
    u8 choice, digitCount;
    u8 inputBuffer[100];
    Patient newPatient;
    u64 tempID;
    u8 keepRunning = 1;
    u32 valid = 1;

    while (keepRunning)
    {
        ScanValidatedName(newPatient.name);

        while (1)
        {
            printf("Enter Age: ");
            fgets(inputBuffer, sizeof(inputBuffer), stdin);

            // Check if the input contains only digits
            
            for (int i = 0; inputBuffer[i] != '\0' && inputBuffer[i] != '\n'; i++)
            {
                if (inputBuffer[i] < '0' || inputBuffer[i] > '9')
                {
                    valid = 0;
                    break;
                }
            }

            if (valid)
            {
                sscanf(inputBuffer, "%hhu", &newPatient.age);
                break;
            }
            else
            {
                printf("Invalid input! Please enter numbers only for age.\n");
            }
        }

       while (1)
       {
           
           printf("Enter Gender (M/F): ");
           fgets(inputBuffer, sizeof(inputBuffer), stdin);
           
           size_t len = strlen(inputBuffer);
           if (len > 0 && inputBuffer[len - 1] == '\n')
           {
               inputBuffer[len - 1] = '\0';
               len--;
           }
           if (len == 1 && (inputBuffer[0] == 'M' || inputBuffer[0] == 'm' || inputBuffer[0] == 'F' || inputBuffer[0] == 'f'))
           {
                       newPatient.gender = (inputBuffer[0] == 'm') ? 'M' :
                                            (inputBuffer[0] == 'f') ? 'F' :
                                            inputBuffer[0];
                                            
                        break;
           }
           else
           {
               printf("Invalid gender! Please enter only 'M' for male or 'F' for female.\n");
           }
       }
       
        while (1)
        {
            valid = 1;
            printf("Enter Unique ID: ");
            fgets(inputBuffer, sizeof(inputBuffer), stdin);
            size_t len = strlen(inputBuffer);
            
            if (len > 0 && inputBuffer[len - 1] == '\n')
            {
                inputBuffer[len - 1] = '\0';
                len--;
            }
            
            if (len != 14)
            {
                printf("Invalid ID! The ID must be exactly 14 digits. You entered %zu digits.\n", len);
                continue;
            }
            
            for (size_t i = 0; i < len; i++)
            {
                if (inputBuffer[i] < '0' || inputBuffer[i] > '9')
                {
                    valid = 0;
                    break;
                }
            }
            
            if (!valid)
            {
                printf("Invalid ID! ID must contain digits only.\n");
                continue;
                
            }
            
            sscanf(inputBuffer, "%llu", &newPatient.ID);
            
            if (IsIDExists(newPatient.ID))
            {
                printf("This ID already exists! Try again.\n");
                continue;
            }
            
            break; // ID valid and unique
            }
            
        patients[patientCount] = newPatient;
        patientCount++;
        printf("Patient record added successfully!\n");

       while (1)
       {
           printf("Do you want to add another patient? (Y/N): ");
           fgets(inputBuffer, sizeof(inputBuffer), stdin);
           
           size_t len = strlen(inputBuffer);
           
           if (len > 0 && inputBuffer[len - 1] == '\n')
           {
               inputBuffer[len - 1] = '\0';
               len--;
           }
           if (len == 1 && (inputBuffer[0] == 'Y' || inputBuffer[0] == 'y'))
           {
               break;
           }
           else if (len == 1 && (inputBuffer[0] == 'N' || inputBuffer[0] == 'n'))
           {
               keepRunning = 0;
               break;
           }
           else
           {
               printf("Invalid input! Please enter 'Y' for yes or 'N' for no.\n");
           }
       }
    }
}

void ScanValidatedName(u8 *str)
{
    while (1)
    {
        printf("Enter Patient Name: ");
        fgets((char *)str, 40, stdin);
        size_t len = strlen((char *)str);
        if (len > 0 && str[len - 1] == '\n')
        {
            str[len - 1] = '\0';
        }

        if (IsNameValid(str))
        {
            break; // name is valid
        }
        else
        {
            printf("Invalid name! Name should contain letters only (no numbers). Try again.\n");
        }
    }
}

u8 IsNameValid(const u8 *name)
{
    for (int i = 0; name[i] != '\0'; i++)
    {
        if ((name[i] >= '0' && name[i] <= '9'))
        {
            return 0; // contains a digit
        }
    }
    return 1; // valid
}

u8 IsIDExists(u64 id)
{
    for (u8 i = 0; i < patientCount; i++)
    {
        if (patients[i].ID == id)
        {
            return 1;
        }
    }
    return 0;
}

s8 FindPatientIndex(u64 id)
{
    for (u8 i = 0; i < patientCount; i++)
    {
        if (patients[i].ID == id)
        {
            return i;
        }
    }
    return -1;
}

void EditPatientRecord(void)
{
    u64 id;
    s8 index;
    u8 inputBuffer[100];
    u32 valid = 1;

    while(1)
    {
        printf("Enter Patient ID to edit: ");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);
        size_t len = strlen(inputBuffer);
        
        if (len > 0 && inputBuffer[len - 1] == '\n')
            {
                inputBuffer[len - 1] = '\0';
                len--;
            }
            
            if (len != 14)
            {
                printf("Invalid ID! The ID must be exactly 14 digits. You entered %zu digits.\n", len);
                continue;
            }
            
            
            for (size_t i = 0; i < len; i++)
            {
                if (inputBuffer[i] < '0' || inputBuffer[i] > '9')
                {
                    valid = 0;
                    break;
                }
            }
            
            if (!valid)
            {
                printf("Invalid ID! ID must contain digits only.\n");
                continue;
                
            }
            
            sscanf(inputBuffer, "%llu", &id);
            
            index = FindPatientIndex(id);
            
            if (index == -1)
            {
                printf("Incorrect ID! patient not found.\n");
            }
            else
            {
                printf("Editing patient ID: %llu\n", id);
                ScanValidatedName(patients[index].name);
                
                while (1)
                {
                    printf("Enter new age: ");
                    fgets(inputBuffer, sizeof(inputBuffer), stdin);
                    
                    // Check if the input contains only digits
                    valid = 1;
                    for (int i = 0; inputBuffer[i] != '\0' && inputBuffer[i] != '\n'; i++)
                    {
                        if (inputBuffer[i] < '0' || inputBuffer[i] > '9')
                        {
                            valid = 0;
                            break;
                        }
                    }
                    if (valid)
                    {
                        sscanf(inputBuffer, "%hhu", &patients[index].age);
                        break;
                    }
                    else
                    {
                        printf("Invalid input! Please enter numbers only for age.\n");
                    }
                }
                
                while (1)
                {
                    printf("Enter new gender (M/F): ");
                    fgets(inputBuffer, sizeof(inputBuffer), stdin);
                    
                    size_t len = strlen(inputBuffer);
                    if (len > 0 && inputBuffer[len - 1] == '\n')
                    {
                        inputBuffer[len - 1] = '\0';
                    }
                    if ((strlen(inputBuffer) == 1) && (inputBuffer[0] == 'M' || inputBuffer[0] == 'm' || inputBuffer[0] == 'F' || inputBuffer[0] == 'f'))
                    {
                        patients[index].gender = toupper(inputBuffer[0]);
                        break;
                    }
                    else
                    {
                        printf("Invalid gender! Please enter only 'M' or 'F'.\n");
                    }
                }
                
                printf("Patient record updated successfully!\n");
                break;
            }
    }
}

void ReserveSlotWithDoctor(void)
{
    u64 id;
    u8 inputBuffer[50];
    u8 choice;
    u8 idValid = 0;
    u8 displayedIndex = 1;
    u8 mapping[5];
    u8 canReserve = 1;
    u8 hasAvailableSlot = 0;

    while (1)
    {
        printf("Enter Patient ID: ");
        fgets(inputBuffer, sizeof(inputBuffer), stdin);
        size_t len = strlen(inputBuffer);
        
        if (len > 0 && inputBuffer[len - 1] == '\n')
        {
            inputBuffer[len - 1] = '\0';
            len--;
        }
        
        if (len != 14)
        {
            printf("Invalid ID! The ID must be exactly 14 digits. You entered %zu digits.\n", len);
            continue;
        }
        
        idValid = 1;
        for (size_t i = 0; i < len; i++)
        {
            if (inputBuffer[i] < '0' || inputBuffer[i] > '9')
            {
                idValid = 0;
                break;
            }
        }
        
        if (!idValid)
        {
            printf("Invalid ID! ID must contain digits only.\n");
            continue;
        }
        
        sscanf(inputBuffer, "%llu", &id);
        break;
    }

    if (!IsIDExists(id))
    {
        printf("ID not found! Please add the patient first.\n");
        canReserve = 0;
    }
    else if (HasExistingReservation(id))
    {
        printf("This patient already has a reserved slot and cannot book another.\n");
        canReserve = 0;
    }

    if (canReserve)
    {
        printf("Available slots:\n");
        for (u8 i = 0; i < 5; i++)
        {
            if (!slots[i].reserved)
            {
                printf("%d. %s\n", displayedIndex, slots[i].time);
                mapping[displayedIndex - 1] = i;
                displayedIndex++;
                hasAvailableSlot = 1;
            }
        }

        if (hasAvailableSlot)
        {
            printf("Enter the number of the desired slot: ");
            fgets(inputBuffer, sizeof(inputBuffer), stdin);
            
            size_t len = strlen(inputBuffer);
            if (len > 0 && inputBuffer[len - 1] == '\n')
            {
                inputBuffer[len - 1] = '\0';
                len--;
            }

            int isNumeric = 1;
            for (size_t i = 0; i < len; i++)
            {
                if (inputBuffer[i] < '0' || inputBuffer[i] > '9')
                {
                    isNumeric = 0;
                    break;
                }
            }
            if (!isNumeric)
            {
                printf("Invalid input! Please enter numbers only without any letters or symbols.\n");
            }
            else
            {
                sscanf(inputBuffer, "%hhu", &choice);
                if (choice > 0 && choice <= displayedIndex - 1)
                {
                    slots[mapping[choice - 1]].reserved = 1;
                    slots[mapping[choice - 1]].patientID = id;
                    printf("Slot reserved successfully.\n");
                }
                else
                {
                    printf("Invalid slot number.\n");
                }
            }
        }
        else
        {
            printf("No slots available for reservation.\n");
        }
    }
}

void initializeSlots(Slot slots[], u8 size)
{
    const u8 *defaultTimes[] = {
        "2:00 PM - 2:30 PM",
        "2:30 PM - 3:00 PM",
        "3:00 PM - 3:30 PM",
        "4:00 PM - 4:30 PM",
        "4:30 PM - 5:00 PM"
    };

    for (u8 i = 0; i < size; i++)
    {
        strcpy(slots[i].time, defaultTimes[i]);
        slots[i].reserved = 0;
        slots[i].patientID = 0;
    }
}

u8 HasExistingReservation(u64 id)
{
    u8 hasReservation = 0;

    for (u8 i = 0; i < 5; i++)
    {
        if (slots[i].reserved && slots[i].patientID == id)
        {
            hasReservation = 1;
            break;
        }
    }

    return hasReservation;
}

void CancelReservation(void)
{
    u64 id;
    u8 found = 0;
    u8 inputBuffer[50];
    u32 valid = 1;

    while (1)
        {
            printf("Enter Patient ID to cancel reservation: ");
            fgets(inputBuffer, sizeof(inputBuffer), stdin);
            size_t len = strlen(inputBuffer);
            
            if (len > 0 && inputBuffer[len - 1] == '\n')
            {
                inputBuffer[len - 1] = '\0';
                len--;
            }
            
            if (len != 14)
            {
                printf("Invalid ID! The ID must be exactly 14 digits. You entered %zu digits.\n", len);
                continue;
            }
            
            for (size_t i = 0; i < len; i++)
            {
                if (inputBuffer[i] < '0' || inputBuffer[i] > '9')
                {
                    valid = 0;
                    break;
                }
            }
            
            if (!valid)
            {
                printf("Invalid ID! ID must contain digits only.\n");
                continue;
                
            }
            
            sscanf(inputBuffer, "%llu", &id);
            break;
        }

    for (u8 i = 0; i < 5; i++)
    {
        if (slots[i].reserved && slots[i].patientID == id)
        {
            slots[i].reserved = 0;
            slots[i].patientID = 0;
            found = 1;
            printf("Reservation cancelled successfully.\n");
            break;
        }
    }

    if (!found)
    {
        printf("No reservation found for the given ID.\n");
    }
}


