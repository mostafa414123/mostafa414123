#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define	MAX_NAME_LEN				40
#define	MIN_NAME_REQ				4
#define	MAX_ADDRESS_LEN				50
#define	MAX_NATIONAL_ID_LEN			15
#define	MAX_BANK_ACC_ID				10
#define	GAURDIAN_AGE_LIMIT			21
#define	MAX_GAURDIAN_NAT_ID_LEN		(MAX_NATIONAL_ID_LEN)
#define	MAX_PASSWORD_LEN			10
#define	TRUE	1
#define	FALSE	0
typedef	enum {
	ACCOUNT_STATUS_DISABLE,
	ACCOUNT_STATUS_ACTIVE,
	ACCOUNT_STATUS_RESTRICTED,
	ACCOUNT_STATUS_CLOSED,
	TOTAL_ACCOUNT_STATUS
}account_status_e;

typedef	enum {
	ADMIN_WINDOW = 1,
	CLIENT_WINDOW,
	TOTAL_ACTIONS_TO_TAKE
}actions_toTake_e;
typedef enum {
	CREATE_NEW_ACCOUNT = 1,
	OPEN_EXISTING_ACCOUNT,
	ADMIN_MODE_EXIT,
	TOTAL_ADMIN_RIGHTS
}admin_right;
typedef enum {
	MAKE_TRANSACTION = 1,
	CHANGE_ACCOUNT_PASSWORD,
	GET_CASH,
	DEPOSITE_IN_ACCOUNT,
	CLIENT_MODE_EXIT,
	TOTAL_CLIENT_RIGHTS
}client_right;
typedef enum {
	OEM_MAKE_TRANSACTION = 1,
	OEM_CHANGE_ACCOUNT_STATUS,
	OEM_GET_CASH,
	OEM_DEPOSITE_IN_ACCOUNT,
	OEM_MODE_EXIT,
	TOTAL_OEM_RIGHTS
}open_existing_menu_e;

typedef unsigned char 		uchar;
typedef unsigned short 		ushort;
typedef unsigned int 		uint;
typedef unsigned long long		ull;

typedef struct bank_acc_info_s
{
	char name[MAX_NAME_LEN];
	char address[MAX_ADDRESS_LEN];
	char national_id[MAX_NATIONAL_ID_LEN];
	ushort  age;
	ull bank_acc_id;
	uchar guardianStatus;
	char guardianID[MAX_GAURDIAN_NAT_ID_LEN];
	account_status_e myAccountSattus;
	uint myBalance;
	uint myPassword;
}bank_acc_info_t;
typedef struct bank_database {
	bank_acc_info_t* bank_database;
	int databaseIndex;
}bank_database_t;
bank_database_t bank_db;
uchar verifyIDnumberIsAllDigit(const char* idstr)
{
	while (*idstr)
	{
		if (isdigit(*idstr++) == 0)
			return (0);
	}
	return (1);
}
void dumpData(void)
{
	int itr = 0;
	if (0 != bank_db.databaseIndex)
	{
		printf("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n");
		printf("| index |\t\t|  name |\t\t| ADDRESS |\t\t| NAT.ID |\t   | AGE |\t|bankNo |\t| GourdianStat |\t| GourdianID |\t| Acc. Stat |\t| Balance |\t| Password |\n\n");
	}
	for (itr = 0; ((0 != bank_db.databaseIndex) && (itr < bank_db.databaseIndex)); itr++)
	{
		bank_acc_info_t* bank_database = &bank_db.bank_database[itr];
		const char acc_Stat[TOTAL_ACCOUNT_STATUS][10] = {
			"Disable",
			"Active",
			"Restricted",
			"Closed"
		};

		printf("\n | %d \t | %s \t| %s \t| %s   | %hu \t| %llu  \t| %s      \t| %s \t| %s \t| %u \t| %u |\n", itr, bank_database->name, bank_database->address,
			bank_database->national_id, bank_database->age, bank_database->bank_acc_id, (bank_database->guardianStatus != FALSE) ? "True" : "False",
			(bank_database->guardianStatus != FALSE) ? bank_database->guardianID : "--------------", acc_Stat[bank_database->myAccountSattus],
			bank_database->myBalance, bank_database->myPassword);
	}
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n");
}

uchar isFirstFour(char* newName)
{
	int i = 0, character = 0, count = 0;
	for (i = 0; i < (strlen(newName) - 1); i++)
	{
		if (newName[i] != ' ')
		{
			if (isalpha(newName[i]) == 0)
				return (1);
			character++;
		}
		else if (newName[i] == ' ')
		{
			if (character >= 1)
			{
				character = 0;
				count++;
			}
		}
	}
	if ((count >= 3) && (character >= 1))
		return (0);
	return (1);
}
uint generateRandomPassword(void)
{
	uint passwordRandom = 0;
	passwordRandom = (rand() % (9999 - 999999999 + 1)) + 9999;
	return (passwordRandom);
}
ull  generateBankID(void)
{
	uchar itrBankNum = 0;
	ull bankNum = 0;
	while (TRUE)
	{
		bankNum = (rand() % (999999999 - 9999999999 + 1)) + 999999999;
		for (itrBankNum = 0; ((0 != bank_db.databaseIndex) && (itrBankNum < bank_db.databaseIndex)); itrBankNum++)
		{
			if (bank_db.bank_database[itrBankNum].bank_acc_id == bankNum)
				break;
		}
		if (itrBankNum == bank_db.databaseIndex)
			break;
	}
	return (bankNum);
}
void updateSecondaryDatabase(void)
{
	FILE* fptrUpdate = NULL;
	if (NULL == (fptrUpdate = fopen("bank_database.txt", "wb")))
	{
		printf("\n\t\t\t\tCan not read database\n");
		exit(0);
	}
	fseek(fptrUpdate, 0, SEEK_SET);
	int itrUpdate = 0;
	for (itrUpdate = 0; ((0 != bank_db.databaseIndex) && (itrUpdate < bank_db.databaseIndex)); itrUpdate++)
	{
		if (!(fwrite(&bank_db.bank_database[itrUpdate], sizeof(bank_acc_info_t), 1, fptrUpdate)))
		{
			printf("\n\t\t\t\tError occurred while adding new entry to the file\n");
			exit(0);
		}
	}
	fclose(fptrUpdate);
	fptrUpdate = NULL;
}
void addNewEntryToFile(bank_acc_info_t* entryToUpdate)
{
	FILE* fptrAdd = NULL;
	if (NULL == (fptrAdd = fopen("bank_database.txt", "ab")))
	{
		printf("\n\t\t\t\tCan not read database\n");
		exit(0);
	}
	fseek(fptrAdd, 0, SEEK_END);
	if (!(fwrite(entryToUpdate, sizeof(bank_acc_info_t), 1, fptrAdd)))
	{
		printf("\n\t\t\t\tError occurred while adding new entry to the file\n");
		exit(0);
	}
	fclose(fptrAdd);
	fptrAdd = NULL;
}

void addAccount(void)
{
	bank_acc_info_t newName;
	size_t ret = 0;
	char* removeNewLine = NULL;
	memset(&newName, 0, sizeof(bank_acc_info_t));
name_correction:
	printf("Enter name : \n");
	fflush(stdin);
	fseek(stdin, 0, SEEK_END);
	fgets(newName.name, MAX_NAME_LEN, stdin);
	if (isFirstFour(newName.name))
	{
		printf("\n\t\t\t\tAtleast first four names expected\n");
		goto name_correction;
	}
	if (removeNewLine = strchr(newName.name, '\n'))
	{
		memmove(removeNewLine, removeNewLine + 1, strlen(removeNewLine));
	}
addr_correction:
	printf("Enter Address :\n");
	fflush(stdin);
	fseek(stdin, 0, SEEK_END);
	fgets(newName.address, MAX_ADDRESS_LEN, stdin);
	ret = strlen(newName.address);
	if (0 == (ret - 1))
	{
		printf("\n\t\t\t\tAddress can not be blank\n");
		goto addr_correction;
	}
	if (removeNewLine = strchr(newName.address, '\n'))
	{
		memmove(removeNewLine, removeNewLine + 1, strlen(removeNewLine));
	}

nationalID_correction:
	printf("Enter 14 Digit  National ID :");
	fflush(stdin);
	fseek(stdin, 0, SEEK_END);
	fgets(newName.national_id, MAX_NATIONAL_ID_LEN, stdin);
	ret = strlen(newName.national_id);
	if (0 == (ret - 1))
	{
		printf("\n\t\t\t\tNational ID can not be blank\n");
		goto nationalID_correction;
	}
	else if ((MAX_NATIONAL_ID_LEN - 1) > ret)
	{
		printf(" \n\t\t\t\tNational ID must be 14 digit long\n");
		goto nationalID_correction;
	}
	else if (!verifyIDnumberIsAllDigit(newName.national_id))
	{
		printf("\n\t\t\t\t National ID must contain digits only\n");
		goto nationalID_correction;
	}
	printf("Enter your AGE : ");
	scanf("%hu", &newName.age);
	newName.bank_acc_id = generateBankID();
	if (GAURDIAN_AGE_LIMIT > newName.age)
	{
	GaurdNationalID_correction:
		printf("Enter 14 Digit Gaurdian's National ID :");
		fflush(stdin);
		fseek(stdin, 0, SEEK_END);
		fgets(newName.guardianID, MAX_NATIONAL_ID_LEN, stdin);
		ret = strlen(newName.guardianID);
		if (0 == (ret - 1))
		{
			printf("\n\t\t\t\tGuardian National ID can not be blank\n");
			goto GaurdNationalID_correction;
		}
		else if ((MAX_NATIONAL_ID_LEN - 1) > ret)
		{
			printf("\n\t\t\t\tGuardian National ID must be 14 digit long\n");
			goto GaurdNationalID_correction;
		}
		else if (!verifyIDnumberIsAllDigit(newName.guardianID))
		{
			printf("\n\t\t\t\t National ID must contain digits only\n");
			goto GaurdNationalID_correction;
		}
		newName.guardianStatus = TRUE;
	}
	else
		newName.guardianStatus = FALSE;
	newName.myAccountSattus = ACCOUNT_STATUS_ACTIVE;
	fflush(stdin);
	fseek(stdin, 0, SEEK_END);
RE_ENTER_AMO:
	printf("Enter Account Opening amount :");
	if (!scanf("%d", &newName.myBalance))
	{
		printf("Invalid Account Opening amount. Enter valid figure\n");
		goto RE_ENTER_AMO;
	}
	newName.myPassword = generateRandomPassword();
	if (0 == bank_db.databaseIndex)
	{
		bank_db.bank_database = (bank_acc_info_t*)calloc(1, sizeof(bank_acc_info_t));
	}
	else
	{
		bank_db.bank_database = (bank_acc_info_t*)realloc(bank_db.bank_database, ((bank_db.databaseIndex + 1) * sizeof(bank_acc_info_t)));
	}
	memcpy(&bank_db.bank_database[bank_db.databaseIndex], &newName, sizeof(bank_acc_info_t));
	addNewEntryToFile(&bank_db.bank_database[bank_db.databaseIndex]);
	bank_db.databaseIndex++;
	dumpData();
}
int verifyBankIDandAvaibility(ull idToverify)
{
	if (0 == bank_db.databaseIndex)
		return (-3);
	if ((999999999 < idToverify) && (9999999999 > idToverify))
	{
		int itrBankID = 0;
		for (itrBankID = 0; itrBankID < bank_db.databaseIndex; itrBankID++)
		{
			if (idToverify == bank_db.bank_database[itrBankID].bank_acc_id)
			{
				return (itrBankID);
			}
		}
		if (itrBankID == bank_db.databaseIndex)
			return (-1);
	}
	return (-2);
}
account_status_e isAccountActive(uchar type, ull account_number)
{
	int itraccact = 0;
	account_status_e status = TOTAL_ACCOUNT_STATUS;
	for (itraccact = 0; itraccact < bank_db.databaseIndex; itraccact++)
	{
		if (account_number == bank_db.bank_database[itraccact].bank_acc_id)
		{
			status = bank_db.bank_database[itraccact].myAccountSattus;
			break;
		}
	}
	if (itraccact == bank_db.databaseIndex)
	{
		(type) ? printf("\n\t\t\t\tBenificiary Bank account is not registered in record\n") : printf("\n\t\t\t\tclient account is not registered in record\n");
		return (-1);
	}
	else if (ACCOUNT_STATUS_DISABLE == status)
	{
		(type) ? printf("\n\t\t\t\tBenificiary account is disabled\n") : printf("\n\t\t\t\tClients account is disabled\n");
		return (-2);
	}
	else if (ACCOUNT_STATUS_RESTRICTED == status)
	{
		(type) ? printf("\n\t\t\t\tBenificiary account is restricted\n") : printf("\n\t\t\t\tClient account is restricted\n");
		return (-3);
	}
	else if (ACCOUNT_STATUS_CLOSED == status)
	{
		(type) ? printf("\n\t\t\t\tBenificiary account is closed\n") : printf("\n\t\t\t\tClient account is closed\n");
		return (-4);
	}
	else
		return (ACCOUNT_STATUS_ACTIVE);

}
int makeTransaction(ull clientAccNum, uint clientindex)
{
	ull beni_acc_no = 0;
	uint amount = 0;
REENTER_BEN_BANK_ID:
	printf("\n\t\t\t\tEnter Benificiary account number : ");
	scanf("%llu", &beni_acc_no);
	if (clientAccNum == beni_acc_no)
	{
		printf("\n\t\t\t\tBenificiary account can not be same as client account.\n");
		return (-3);
	}
	int retBenBankID = verifyBankIDandAvaibility(beni_acc_no);
	if (-1 == retBenBankID)
	{
		printf("\n\t\t\t\tBenificiary Bank account is not registered in record\n");
		goto REENTER_BEN_BANK_ID;
	}
	else if (-2 == retBenBankID)
	{
		printf("\n\t\t\t\tEnter correct Bank ID.\n");
		goto REENTER_BEN_BANK_ID;
	}
	if (ACCOUNT_STATUS_ACTIVE != isAccountActive(1, beni_acc_no))
	{
		return(-1);
	}
	if (ACCOUNT_STATUS_ACTIVE != isAccountActive(0, clientAccNum))
	{
		return(-1);
	}
	printf("\n\t\t\t\tEnter amount to transfer : ");
	scanf("%d", &amount);
	if (bank_db.bank_database[clientindex].myBalance < amount)
	{
		printf("\n\t\t\t\tCan not proceed due to low balance\n");
		return (-2);
	}
	else
	{
		bank_db.bank_database[clientindex].myBalance -= amount;
		bank_db.bank_database[retBenBankID].myBalance += amount;
		updateSecondaryDatabase();
	}
	printf("\n\n\t\t\t\t###################################\n\n");
	printf("\n\t\t\t\tTransaction successful...!!");
	printf("\n\n\t\t\t\t###################################\n\n");
	return (0);
}
void changeAccountStatus(int retBankIDCl)
{
	printf("\n\n\n\t\t\t\tSelect state to change");
	printf("\n\n\n\t\t\t\t*********************************\n\n\t\t\t\t1. Active\n\t\t\t\t2. Restricted\n\t\t\t\t3. Close\n\n\t\t\t\t*********************************\n\n");
	ushort state = 0;
	scanf("%hu", &state);
	bank_db.bank_database[retBankIDCl].myAccountSattus = state;
	updateSecondaryDatabase();
}
void getClientCashStatus(int retBankIDcash)
{
	uint cashAmount = 0;
	printf("\n\n\n\t\t\t\tEnter cash amount : ");
	scanf("%u", &cashAmount);
	if (bank_db.bank_database[retBankIDcash].myBalance < cashAmount)
	{
		printf("\n\t\t\t\tUnable to process due to insufficient balance\n");
	}
	else
	{
		bank_db.bank_database[retBankIDcash].myBalance -= cashAmount;
		printf("\n\n\t\t\t\t###################################\n");
		printf("\n\t\t\t\tCash amount %u debited from Your Account.\n\t\t\t\tAvailable Balance : %u\n", cashAmount, bank_db.bank_database[retBankIDcash].myBalance);
		printf("\n\n\t\t\t\t###################################\n");
		updateSecondaryDatabase();
	}
}
void depositToClient(int retBankIDdepo)
{
	uint depoAmount = 0;
	printf("\n\n\n\t\t\t\tEnter deposit amount : ");
	if (0 == scanf("%u", &depoAmount))
	{
		printf("Invalid amount entered,Can not process\n");
		return;
	}
	bank_db.bank_database[retBankIDdepo].myBalance += depoAmount;
	printf("\n\n\t\t\t\t###################################\n");
	printf("\n\t\t\t\tAmount %u added to your Account.\n\t\t\t\tAvailable Balance : %u\n", depoAmount, bank_db.bank_database[retBankIDdepo].myBalance);
	printf("\n\n\t\t\t\t###################################\n");
	updateSecondaryDatabase();
}
void changeMyPassword(int retBankIDpass)
{
	uint newPassword = 0;
#if 0
	uint oldPassword = 0;
	printf("\n\n\n\t\t\t\tEnter old password : ");
	scanf("%u", &oldPassword);
	if (bank_db.bank_database[retBankIDpass].myPassword == oldPassword)
	{
		printf("\n\n\n\t\t\t\tEnter 10 digit new password : ");
		scanf("%u", &newPassword);
		if ((newPassword > 9999) && (newPassword < 999999999))
			bank_db.bank_database[retBankIDpass].myPassword = newPassword;
	}
#endif
RE_ENTER_PASSWORD:
	printf("\n\n\n\t\t\t\tEnter 10 digit new password : ");
	scanf("%u", &newPassword);
	if ((newPassword > 9999) && (newPassword < 999999999))
	{
		bank_db.bank_database[retBankIDpass].myPassword = newPassword;
		updateSecondaryDatabase();
	}
	else
	{
		printf("\n\t\t\t\tNew password should contain  minimum 5 digit or maximum 10 digit\n");
		goto RE_ENTER_PASSWORD;
	}
}
void init_database(void)
{
	FILE* fptr = NULL;
	bank_acc_info_t readFile;
	memset(&readFile, 0, sizeof(bank_acc_info_t));
	bank_db.databaseIndex = 0;
	bank_db.bank_database = NULL;
	if (NULL == (fptr = fopen("bank_database.txt", "rb")))
	{
		return;
	}
	while (fread(&readFile, sizeof(readFile), 1, fptr))
	{
		if (0 == bank_db.databaseIndex)
		{
			bank_db.bank_database = (bank_acc_info_t*)calloc(1, sizeof(bank_acc_info_t));
		}
		else
		{
			bank_db.bank_database = (bank_acc_info_t*)realloc(bank_db.bank_database, ((bank_db.databaseIndex + 1) * sizeof(bank_acc_info_t)));
		}
		memcpy(&bank_db.bank_database[bank_db.databaseIndex++], &readFile, sizeof(bank_acc_info_t));
	}
	fclose(fptr);
	fptr = NULL;
	dumpData();
}

int main(void)
{
	ushort choice = 0;
	int retBankID = 0;
	ull clientBankID = 0;
	init_database();
	while (TRUE)
	{
		printf("\tSelect your choice to process : \n\t\t\t1.	Admin Window\n\t\t\t2.	Client Window\n");
		fflush(stdin);
		fseek(stdin, 0, SEEK_END);
		scanf("%hu", &choice);
		if (ADMIN_WINDOW == choice)
		{
			ushort adminChoice = 0;
			printf("\n\t\t\t*********************************	Admin Window	*********************************\n");
			while (TRUE)
			{
			JUMP_MAIN_MENU:
				printf("\n\n\n\t\t\t\t*********************************\n\n\t\t\t\t1. Create New Account\n\t\t\t\t2. Open Existing Account\n\t\t\t\t3. Exit\n\n\t\t\t\t*********************************\n\n");
				fflush(stdin);
				fseek(stdin, 0, SEEK_END);
				scanf("%hu", &adminChoice);
				if (CREATE_NEW_ACCOUNT == adminChoice)
				{
					addAccount();
				}
				else if (OPEN_EXISTING_ACCOUNT == adminChoice)
				{
					ushort oea_choice = 0;
				REENTER_BANK_ID:
					printf("\n\t\t\t\tEnter Bank ID :");
					scanf("%llu", &clientBankID);
					retBankID = verifyBankIDandAvaibility(clientBankID);
					if (-1 == retBankID)
					{
						printf("\n\t\t\t\tBank ID is not registered in record\n");
						goto REENTER_BANK_ID;
					}
					else if (-2 == retBankID)
					{
						printf("\n\t\t\t\tEnter correct Bank ID.\n");
						goto REENTER_BANK_ID;
					}
					else if (-3 == retBankID)
					{
						printf("\n\t\t\t\tNo bank account is registered\n");
						goto JUMP_MAIN_MENU;
					}
					while (TRUE)
					{
						printf("\n\n\n\t\t\t\t*********************************\n\n\t\t\t\t1.  Make Transaction\n\t\t\t\t2.  Change Account Status\n\t\t\t\t3.  Get Cash\n\t\t\t\t4.  Deposit\n\t\t\t\t5.  Return\n\n\t\t\t\t*********************************\n\n");
						fflush(stdin);
						fseek(stdin, 0, SEEK_END);
						scanf("%hu", &oea_choice);
						if (OEM_MAKE_TRANSACTION == oea_choice)
						{
							makeTransaction(clientBankID, retBankID);
						}
						else if (OEM_CHANGE_ACCOUNT_STATUS == oea_choice)
						{
							changeAccountStatus(retBankID);
						}
						else if (OEM_GET_CASH == oea_choice)
						{
							getClientCashStatus(retBankID);
						}
						else if (OEM_DEPOSITE_IN_ACCOUNT == oea_choice)
						{
							depositToClient(retBankID);
						}
						else if (OEM_MODE_EXIT == oea_choice)
						{
							break;
						}
						else
							printf("\n\t\t\tInvalid choice please enter correct number respective to action you wish to perform...");
						oea_choice = 0;
					}
				}
				else if (ADMIN_MODE_EXIT == adminChoice)
				{
					break;
				}
				else
					printf("\n\t\t\tInvalid choice please enter correct number respective to action you wish to perform...");

				adminChoice = 0;
			}
		}
		else if (CLIENT_WINDOW == choice)
		{
			ushort clientChoice = 0;
			printf("\n\t\t\t*********************************   Client Window   *********************************\n");
			printf("\n\t\t\tEnter Bank ID :");
			fflush(stdin);
			fseek(stdin, 0, SEEK_END);
			scanf("%llu", &clientBankID);
			retBankID = verifyBankIDandAvaibility(clientBankID);
			if (-1 == retBankID)
			{
				printf("\n\t\t\tBank ID is not registered in record\n");
				goto REENTER_BANK_ID;
			}
			else if (-2 == retBankID)
			{
				printf("\n\t\t\tEnter correct Bank ID.\n");
				goto REENTER_BANK_ID;
			}
			else if (-3 == retBankID)
			{
				printf("\n\t\t\t\tNo bank account is registered\n");
				goto JUMP_MAIN_MENU;
			}
			while (TRUE)
			{
				printf("\n\n\n\t\t\t\t*********************************\n\n\t\t\t\t1.	Make Transaction\n\t\t\t\t2.	Change Account Password\n\t\t\t\t3.	Get Cash\n\t\t\t\t4.	Deposit\n\t\t\t\t5.	Return\n\n\t\t\t\t*********************************\n\n");
				fflush(stdin);
				fseek(stdin, 0, SEEK_END);
				scanf("%hu", &clientChoice);
				if (MAKE_TRANSACTION == clientChoice)
				{
					makeTransaction(clientBankID, retBankID);
				}
				else if (CHANGE_ACCOUNT_PASSWORD == clientChoice)
				{
					changeMyPassword(retBankID);
				}
				else if (GET_CASH == clientChoice)
				{
					getClientCashStatus(retBankID);
				}
				else if (DEPOSITE_IN_ACCOUNT == clientChoice)
				{
					depositToClient(retBankID);
				}
				else if (CLIENT_MODE_EXIT == clientChoice)
				{
					break;
				}
				else
					printf("\n\t\t\tInvalid choice please enter correct number respective to action you wish to perform...");
				clientChoice = 0;
			}
		}
		else
			printf("\n\t\t\tInvalid choice please enter correct number respective to action you wish to perform...");
		choice = 0;
	}

}

