#include "common.h"

static char software_version[] = "$Id: print.c,v 1.8 2003-08-15 07:10:41 freddy77 Exp $";
static void *no_unused_var_warn[] = { software_version, no_unused_var_warn };

static SQLCHAR output[256];
static void ReadError(void);

static void
ReadError(void)
{
	if (!SQL_SUCCEEDED(SQLGetDiagRec(SQL_HANDLE_STMT, Statement, 1, NULL, NULL, output, sizeof(output), NULL))) {
		printf("SQLGetDiagRec should not fail\n");
		exit(1);
	}
	printf("Message: %s\n", output);
}

int
main(int argc, char *argv[])
{
	SQLINTEGER cnamesize;
	const char *command;

	Connect();

	/* issue print statement and test message returned */
	command = "print 'Test message'";
	printf("%s\n", command);
	if (SQLExecDirect(Statement, (SQLCHAR *) command, SQL_NTS) != SQL_SUCCESS_WITH_INFO) {
		printf("SQLExecDirect should return SQL_SUCCESS_WITH_INFO\n");
		return 1;
	}
	ReadError();
	if (!strstr((char *) output, "Test message")) {
		printf("Message invalid\n");
		return 1;
	}

	/* issue invalid command and test error */
	command = "SELECT donotexistsfield FROM donotexiststable";
	printf("%s\n", command);
	if (SQLExecDirect(Statement, (SQLCHAR *) command, SQL_NTS) != SQL_ERROR) {
		printf("SQLExecDirect returned strange results\n");
		return 1;
	}
	ReadError();

	/* test no data returned */
	if (SQLFetch(Statement) != SQL_ERROR) {
		printf("Row fetched ??\n");
		return 1;
	}
	ReadError();

	if (SQLGetData(Statement, 1, SQL_C_CHAR, output, sizeof(output), &cnamesize) != SQL_ERROR) {
		printf("Data ??\n");
		return 1;
	}
	ReadError();

	Disconnect();

	printf("Done.\n");
	return 0;
}
