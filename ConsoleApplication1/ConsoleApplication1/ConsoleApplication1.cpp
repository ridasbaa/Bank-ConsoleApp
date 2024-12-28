#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>

using namespace std;

void MainMenuScreen();
void TransactionScreen();
void ManageUsersScreen();
void Login();

short ChooseOption(short From, short To) {

	short Answer = 0;

	do
	{
		cout << "Choose what do you want to do? [" << From << " to " << To << "] ? : ";
		cin >> Answer;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

	} while (Answer < From || Answer > To);

	return Answer;

}
string ReadString(string Message) {

	string Text;
	cout << Message;
	getline(cin >> ws, Text);
	return Text;
}

string ClientFileName = "BankAccountsInfo.txt";
string UsersFileName = "UsersInfo.txt";

enum enTransactionOptions { eDeposite = 1, eWithdraw = 2, eTotalBalance = 3, eMainMenu = 4 };
enum enUserOption { eUsersList = 1, eAddUser = 2, eDeleteUser = 3, eUpdateUser = 4, eFindUser = 5, eMenu = 6 };
enum enOptions { eShowClients = 1, eAddClient = 2, eDeleteClient = 3, eUpdateClient = 4, eFindClient = 5, eTransactions = 6, eManageUsers = 7, eLogOut = 8 };
enum enMainMenuPermissions { eAll = -1, pClientList = 1, pAddClient = 2, pDeleteClient = 4, pUpdateClient = 8, pFindClient = 16, pTransactions = 32, pManageUsers = 64 };

struct stClient {


	string AccountNumber;
	string PinCode;
	string Name;
	string Phone;
	double AccountBalance = 0.00;
	bool MarkToDelete = false;
};
struct stUser {
	string UserName;
	string Password;
	short Permissions = 0;
	bool MarkToDelete = false;
};

stUser CurrentUser;

vector<string>SplitString(string Line, string delim = "#//#") {

	vector<string>vTokens;

	short pos = 0;
	string sWord = "";

	while ((pos = Line.find(delim)) != std::string::npos)
	{
		sWord = Line.substr(0, pos);
		if (!sWord.empty())
		{
			vTokens.push_back(sWord);
		}

		Line.erase(0, pos + delim.length());

	}

	if (!Line.empty())
	{
		vTokens.push_back(Line);
	}
	return vTokens;
}
stClient ConvertLineToRecord(string Line, string delim = "#//#") {

	vector<string>vTokens = SplitString(Line, delim);
	stClient Client;

	Client.AccountNumber = vTokens[0];
	Client.PinCode = vTokens[1];
	Client.Name = vTokens[2];
	Client.Phone = vTokens[3];
	Client.AccountBalance = stod(vTokens[4]);

	return Client;

}
string ConvertRecordToLine(stClient ClientInfo, string Separator = "#//#") {

	string LineTheInfo = "";
	LineTheInfo += ClientInfo.AccountNumber + Separator;
	LineTheInfo += ClientInfo.PinCode + Separator;
	LineTheInfo += ClientInfo.Name + Separator;
	LineTheInfo += ClientInfo.Phone + Separator;
	LineTheInfo += to_string(ClientInfo.AccountBalance);

	return LineTheInfo;
}
vector<stClient>LoadDataFromFile(string FileName) {

	vector<stClient>vClients;
	fstream MyFile;
	stClient Client;

	MyFile.open(FileName, ios::in);

	if (MyFile.is_open())
	{
		string Line = "";
		while (getline(MyFile, Line))
		{
			Client = ConvertLineToRecord(Line);
			vClients.push_back(Client);
		}
		MyFile.close();

	}
	return vClients;
}

void ShowAccessDeniedScreen() {


	system("cls");
	cout << "\n----------------------------------\n";
	cout << "Access Denied\n";
	cout << "You don't have permission to do this.\n";
	cout << "Please Contact your admin";
	cout << "\n----------------------------------\n\n";

	system("pause");
}
bool CheckAccessPermission(enMainMenuPermissions Permission) {

	if (CurrentUser.Permissions == enMainMenuPermissions::eAll) {
		return true;
	}
	else if ((Permission & CurrentUser.Permissions) == Permission) {
		return true;
	}
	return false;
}

bool ClientExistsByAccountNumber(string AccountNumber, string FileName) {

	fstream MyFile;
	
	MyFile.open(FileName, ios::in);

	if (MyFile.is_open())
	{
		string Line;
		stClient Client;
		while (getline(MyFile, Line))
		{
			Client = ConvertLineToRecord(Line);
			if (Client.AccountNumber == AccountNumber)
			{
				MyFile.close();
				return 1;
			}
		}

		MyFile.close();
	}

	return 0;
}

stClient GetClientInfo() {

	stClient Info;

	// usage of STD::ws will extract all the whitespaces characters
	cout << "Enter account Number? : ";
	getline(cin >> ws, Info.AccountNumber);

	while (ClientExistsByAccountNumber(Info.AccountNumber, ClientFileName))
	{
		cout << "\nClient with Account Number [" << Info.AccountNumber << "] already Exists, Please enter another account number : ";
		getline(cin >> ws, Info.AccountNumber);
	}

	cout << "Enter Pin Code? : ";
	getline(cin, Info.PinCode);

	cout << "Enter Your Name? : ";
	getline(cin, Info.Name);

	cout << "Enter Phone? : ";
	getline(cin, Info.Phone);

	cout << "Enter Account Balance? : ";
	cin >> Info.AccountBalance;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	return Info;
}

bool IsClientFound(string AccountNumber, vector<stClient>& vClients, stClient& ClientFound) {

	for (stClient &C : vClients)
	{
		if (C.AccountNumber == AccountNumber)
		{
			ClientFound = C;
			return 1;
		}
	}

	return 0;
}

void PrintClientCard(stClient Client) {

	cout << endl;
	cout << "The following are the client info : \n";
	cout << "Account Number : " << Client.AccountNumber << endl;
	cout << "Pin Code : " << Client.PinCode << endl;
	cout << "Name : " << Client.Name << endl;
	cout << "Phone : " << Client.Phone << endl;
	cout << "Account Balance : " << Client.AccountBalance << endl;

}

void FindClients(string AccountNumber, vector<stClient>& vClients) {

	stClient ClientFound;

	if (IsClientFound(AccountNumber, vClients, ClientFound))
	{
		PrintClientCard(ClientFound);
	}
	else
	{
		cout << "\nThe Client with the account number (" << AccountNumber << ") Not Found!" << endl;
	}

}

void FillFileWithInfo(string Info, string FileName) {

	fstream MyFile;

	MyFile.open(FileName, ios::out | ios::app);

	if (MyFile.is_open())
	{
		MyFile << Info << endl;

		MyFile.close();
	}

}

void AddClients(vector<stClient>& vClients) {

	stClient Client;
	char answer = ' ';

	do
	{
		system("cls");
		cout << "Adding New Clients : \n\n";

		Client = GetClientInfo();
		vClients.push_back(Client);
		FillFileWithInfo(ConvertRecordToLine(Client), ClientFileName);

		cout << "\nClient added successfully, do you want to add more clients? if yes:[y/Y] : ";
		cin >> answer;

	} while (tolower(answer) == 'y');

}

void SaveClientsDataInFile(vector<stClient>vClients) {

	fstream MyFile;

	MyFile.open("BankAccountsInfo.txt", ios::out);

	if (MyFile.is_open())
	{

		string Line = "";
		for (stClient& C : vClients)
		{
			MyFile << ConvertRecordToLine(C) << endl;
		}
	}

}

bool UpdateConfirmation() {

	char answer = ' ';

	cout << "\nAre you sure you want to update this client info : ";
	cin >> answer;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	if (tolower(answer) == 'y')
	{
		return 1;
	}
	return 0;
}
void ChangeClientRecord(stClient& Client) {

	cout << "\nYou are now updating this client info : \n";
	cout << "Enter New Pin Code : ";
	getline(cin >> ws, Client.PinCode);
	cout << "Enter New Name : ";
	getline(cin, Client.Name);
	cout << "Enter New Phone : ";
	getline(cin, Client.Phone);
	cout << "Enter Balance : ";
	cin >> Client.AccountBalance;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
void UpdateClientInfo(string AccountNumber, vector<stClient>& vClients) {

	stClient ClientFound;
	if (IsClientFound(AccountNumber, vClients, ClientFound))
	{
		PrintClientCard(ClientFound);

		if (UpdateConfirmation())
		{
			for (stClient& C : vClients)
			{
				if (C.AccountNumber == AccountNumber) {
					ChangeClientRecord(C);
					break;
				}
			}

			SaveClientsDataInFile(vClients);
		}

		cout << "\nClient Info Updated Successfully!" << endl;
	}
	else
	{
		cout << "The Client with Account Number (" << AccountNumber << "), Not Found!" << endl;
	}


}

bool DeleteConfirmation() {

	char answer = ' ';

	cout << "\nAre you sure you want to delete this client : ";
	cin >> answer;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	if (tolower(answer) == 'y')
	{
		return 1;
	}
	return 0;

}
void DeleteClientFromFile(vector<stClient>& vClients) {

	fstream MyFile;

	MyFile.open("BankAccountsInfo.txt", ios::out);

	if (MyFile.is_open())
	{

		for (stClient& C : vClients)
		{
			if (C.MarkToDelete == false)
			{
				MyFile << ConvertRecordToLine(C) << endl;
			}
		}

		MyFile.close();

	}

}
void DeleteClients(string AccountNumber, vector<stClient>& vClients) {

	stClient ClientFound;

	if (IsClientFound(AccountNumber, vClients, ClientFound))
	{
		PrintClientCard(ClientFound);

		if (DeleteConfirmation()){

			for (stClient& C : vClients)
			{
				if (C.AccountNumber == AccountNumber)
				{
					C.MarkToDelete = true;
				}
			}
			DeleteClientFromFile(vClients);
		}

		cout << "\nClient Deleted Successfully\n";
	}
	else
	{
		cout << "\nThe client with this account number (" << AccountNumber << "), Not Found!\n";
	}
}

void DeleteClientScreen() {

	if (!CheckAccessPermission(enMainMenuPermissions::pDeleteClient)) {
		ShowAccessDeniedScreen();
		return;
	}

	cout << "\n----------------------------------\n";
	cout << "\tDelete Client Screen";
	cout << "\n----------------------------------\n";

	vector<stClient>vClients = LoadDataFromFile(ClientFileName);
	string AccountNumber = ReadString("Enter the account number of the client you want to delete : ");
	DeleteClients(AccountNumber, vClients);
}
void AddClientScreen() {

	if (!CheckAccessPermission(enMainMenuPermissions::pAddClient)) {
		ShowAccessDeniedScreen();
		return;
	}

	cout << "\n----------------------------------\n";
	cout << "\nAdd Clients Screen";
	cout << "\n----------------------------------\n";
	vector<stClient>vClients = LoadDataFromFile(ClientFileName);
	AddClients(vClients);


}
void UpdateClientScreen() {

	if (!CheckAccessPermission(enMainMenuPermissions::pUpdateClient)) {
		ShowAccessDeniedScreen();
		return;
	}

	cout << "\n----------------------------------\n";
	cout << "\tUpdate Client Screen";
	cout << "\n----------------------------------\n";

	vector<stClient>vClients = LoadDataFromFile(ClientFileName);
	string AccountNumber = ReadString("Enter the account number of the client you want to Update: ");
	UpdateClientInfo(AccountNumber, vClients);


	

}
void FindClientScreen() {

	if (!CheckAccessPermission(enMainMenuPermissions::pFindClient)) {
		ShowAccessDeniedScreen();
		return;
	}

	cout << "\n----------------------------------\n";
	cout << "\tFind Client Screen";
	cout << "\n----------------------------------\n";

	vector<stClient>vClients = LoadDataFromFile(ClientFileName);
	string AccountNumber = ReadString("Enter the account number of the client you are looking for : ");
	FindClients(AccountNumber, vClients);


}

void PrintClientData(stClient Client) {

	cout << "| " << left << setw(14) << Client.AccountNumber;
	cout << "| " << left << setw(9) <<  Client.PinCode;
	cout << "| " << left << setw(22) << Client.Name;
	cout << "| " << left << setw(14) << Client.Phone;
	cout << "| " << left << setw(12) << Client.AccountBalance;
	cout << endl;
}
void ShowClientsList() {

	if (!CheckAccessPermission(enMainMenuPermissions::pClientList)) {
		ShowAccessDeniedScreen();
		return;
	}

	vector<stClient>vClients = LoadDataFromFile(ClientFileName);

	cout << "\t\t\tClient List (" << vClients.size() << ") Client (s)" << endl;
	cout << "-------------------------------------------------------------------------------------\n";
	cout << "| " << left << setw(14) << "Account Number";
	cout << "| " << left << setw(9) << "Pin Code";
	cout << "| " << left << setw(22) << "Name";
	cout << "| " << left << setw(14) << "Phone";
	cout << "| " << left << setw(12) << "Account Balance";
	cout << "\n-------------------------------------------------------------------------------------\n";


	for (stClient& C : vClients)
	{
		PrintClientData(C);
	}
	cout << "-------------------------------------------------------------------------------------\n";

}

void GoBacKToTransactionMenu() {

	cout << "Press any key to get back to the main Screen...";
	system("pause");
	TransactionScreen();

}
bool DepositeBalanceByAccountNumber(string AccountNumber, double Amount, vector<stClient>& vClients) {

	char answer = ' ';

	cout << "\nAre you sure you want perform this transaction: ";
	cin >> answer;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	if (tolower(answer) == 'y')
	{
		for (stClient& C : vClients) {

			if (C.AccountNumber == AccountNumber) {
				C.AccountBalance += Amount;
				SaveClientsDataInFile(vClients);
				cout << "\nDone successfully! New balance is : " << C.AccountBalance << endl;
				return true;
			}

		}

		return false;
	}
	else {
		GoBacKToTransactionMenu();
	}

	return false;

}
void Deposite(vector<stClient>& vClients) {

	stClient ClientFound;
	string AccountNumber;

	do {
		AccountNumber = ReadString("Enter The Account Number : ");
	} while (!IsClientFound(AccountNumber, vClients, ClientFound));

	PrintClientCard(ClientFound);

	double Amount = 0;
	cout << "\nPlease enter Deposite amount : ";
	cin >> Amount;

	DepositeBalanceByAccountNumber(AccountNumber, Amount, vClients);
}
void Withdraw(vector<stClient>& vClients)  {

	stClient ClientFound;
	string AccountNumber;

	do {
		AccountNumber = ReadString("Enter The Account Number : ");
	} while (!IsClientFound(AccountNumber, vClients, ClientFound));

	PrintClientCard(ClientFound);

	double Amount = 0;
	do {

		if (ClientFound.AccountBalance < Amount) {
			cout << "\nAmount Exceeds the balance, you can withdraw up to " << ClientFound.AccountBalance << endl;
		}

		cout << "\nPlease enter Withdrawal amount : ";
		cin >> Amount;

	} while (ClientFound.AccountBalance < Amount);

	DepositeBalanceByAccountNumber(AccountNumber, Amount * -1, vClients);


}
void PrintBalance(stClient Client) {

	cout << "| " << left << setw(18) << Client.AccountNumber;
	cout << "| " << left << setw(30) << Client.Name;
	cout << "| " << left << setw(18) << Client.AccountBalance;
	cout << endl;

}

void DepositeScreen() {

	cout << "\n----------------------------------\n";
	cout << "\tDeposite Screen";
	cout << "\n----------------------------------\n";

	vector<stClient>vClients = LoadDataFromFile(ClientFileName);
	Deposite(vClients);
}
void WithdrawScreen() {

	cout << "\n----------------------------------\n";
	cout << "\tWithDraw Screen";
	cout << "\n----------------------------------\n";

	vector<stClient>vClients = LoadDataFromFile(ClientFileName);
	Withdraw(vClients);
}
void TotalBalancesScreen() {

	vector<stClient>vClients = LoadDataFromFile(ClientFileName);

	cout << "\t\t\tBalances List (" << vClients.size() << ") Client(s)." << endl;
	cout << "-------------------------------------------------------------------------------\n";
	cout << "| " << left << setw(18) << "Account Number";
	cout << "| " << left << setw(30) << "Client Name";
	cout << "| " << left << setw(18) << "Balance";
	cout << "\n-------------------------------------------------------------------------------\n";

	for (stClient& Clinet : vClients) {
		PrintBalance(Clinet);
	}

	cout << "\n-------------------------------------------------------------------------------\n";

	double TotalBalances = 0;
	for (stClient& Client : vClients) {

		TotalBalances += Client.AccountBalance;

	}

	cout << "\n\t\t\tTotal balances = " << TotalBalances << endl << endl;
}


string ConvertUserRecordToLine(stUser User, string delim = "#//#") {

	string UserRecordLine = "";

	UserRecordLine = User.UserName + delim;
	UserRecordLine += User.Password + delim;
	UserRecordLine += to_string(User.Permissions);

	return UserRecordLine;

}
stUser ConvertLineToUserRecord(string Line, string delim = "#//#") {

	vector<string>vUser = SplitString(Line, delim);
	stUser User;

	User.UserName = vUser[0];
	User.Password = vUser[1];
	User.Permissions = stoi(vUser[2]);

	return User;

}
vector<stUser> LoadUsersFromFile(string FileName) {

	vector<stUser>vUsers;
	
	fstream MyFile;
	MyFile.open(FileName, ios::in);

	if (MyFile.is_open()) {

		string Line = "";
		stUser User;
		while (getline(MyFile, Line)) {

			User = ConvertLineToUserRecord(Line);
			vUsers.push_back(User);
		}
		MyFile.close();
	}
	return vUsers;
}
bool UserExistByUsername(string FileName, string Username) {

	fstream MyFile;
	MyFile.open(FileName, ios::in);

	if (MyFile.is_open()) {

		string Line = "";
		stUser User;
		while (getline(MyFile, Line)) {

			User = ConvertLineToUserRecord(Line);
			if (User.UserName == Username) {
				MyFile.close();
				return true;
			}

		}

		MyFile.close();
	}
	return false;
}
bool IsUserFound(vector<stUser>& vUsers, string Username, stUser& UserFound) {

	for (stUser& U : vUsers) {

		if (U.UserName == Username) {
			UserFound = U;
			return true;
		}

	}
	return false;
}
void PrintUsersData(stUser User) {

	cout << "| " << left << setw(14) << User.UserName;
	cout << "| " << left << setw(9) << User.Password;
	cout << "| " << left << setw(22) << User.Permissions;
	cout << endl;
}
void ShowUsersList() {

	vector<stUser>vUsers = LoadUsersFromFile(UsersFileName);

	cout << "\t\t\tUsers List (" << vUsers.size() << ") Client (s)" << endl;
	cout << "-------------------------------------------------------------------------------------\n";
	cout << "| " << left << setw(14) << "Username";
	cout << "| " << left << setw(9) << "Password";
	cout << "| " << left << setw(22) << "Permisions";
	cout << "\n-------------------------------------------------------------------------------------\n";


	for (stUser& C : vUsers)
	{
		PrintUsersData(C);
	}
	cout << "-------------------------------------------------------------------------------------\n";


}


stUser GetUserInfo() {

	stUser User;

	cout << "Enter Username : ";
	getline(cin >> ws, User.UserName);

	while (UserExistByUsername(UsersFileName, User.UserName)) {

		cout << "\nThe Username you entered is already Exist!, Please try another Username : " << endl;
		getline(cin >> ws, User.UserName);

	}

	cout << "\nEnter Password : ";
	getline(cin >> ws, User.Password);

	char answer = ' ';

	cout << "\nDo you want to give this user full acess? y/n : ";
	cin >> answer;

	if (tolower(answer) == 'y') {
		User.Permissions += enMainMenuPermissions::eAll;

	}
	else {
		cout << "\nDo you want to give access to : ";

		cout << "\nShow Clients List? y/n : ";
		cin >> answer;
		if (tolower(answer) == 'y') {
			User.Permissions += enMainMenuPermissions::pClientList;
		}

		cout << "\nAdd New Client : ";
		cin >> answer;
		if (tolower(answer) == 'y') {
			User.Permissions += enMainMenuPermissions::pAddClient;

		}

		cout << "\nDelete Client : ";
		cin >> answer;
		if (tolower(answer) == 'y') {
			User.Permissions += enMainMenuPermissions::pDeleteClient;
		}

		cout << "\n Update Client : ";
		cin >> answer;
		if (tolower(answer) == 'y') {
			User.Permissions += enMainMenuPermissions::pUpdateClient;
		}

		cout << "\nFind Client : ";
		cin >> answer;
		if (tolower(answer) == 'y') {
			User.Permissions += enMainMenuPermissions::pFindClient;
		}

		cout << "\n Transactions: ";
		cin >> answer;
		if (tolower(answer) == 'y') {
			User.Permissions += enMainMenuPermissions::pTransactions;
		}

		cout << "\n Manage Users : ";
		cin >> answer;
		if (tolower(answer) == 'y') {
			User.Permissions += enMainMenuPermissions::pManageUsers;
		}

	}

	return User;

}
void AddUser(vector<stUser>&vUsers) {

	stUser User;
	char answer = ' ';
	do {
		system("cls");
		cout << "Adding User Info :\n";
		User = GetUserInfo();
		vUsers.push_back(User);
		FillFileWithInfo(ConvertUserRecordToLine(User), UsersFileName);

		cout << "\nUser Added successfully, Do you want to add more users Yes:[Y/n] : ";
		cin >> answer;

	} while (tolower(answer) == 'y');

}
void AddUsersScreen() {

	cout << "\n----------------------------------\n";
	cout << "\tAdd Usres Screen";
	cout << "\n----------------------------------\n";
	vector<stUser>vUsers= LoadUsersFromFile(UsersFileName);
	AddUser(vUsers);

}

void PrintUserCard(stUser User) {

	cout << endl;
	cout << "-------------------------------------\n";
	cout << "Username : " << User.UserName << endl;
	cout << "Password : " << User.Password << endl;
	cout << "Permissions : " << User.Permissions << endl;
	cout << "-------------------------------------\n";
}

void DeleteUserFromFile(vector<stUser>& vUsers) {

	fstream MyFile;

	MyFile.open(UsersFileName, ios::out);

	if (MyFile.is_open()) {

		for (stUser& U : vUsers) {

			if (U.MarkToDelete == false) {
				MyFile << ConvertUserRecordToLine(U) << endl;
				
			}

		}

		MyFile.close();
	}

}
bool DeleteUserConfirmation() {

	char answer = ' ';

	cout << "\nAre you sure you want to delete this User : ";
	cin >> answer;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	if (tolower(answer) == 'y')
	{
		return 1;
	}
	return 0;

}
void DeleteUser(vector<stUser>& vUsers) {

	string Username;
	stUser UserFound;

	cout << "\nEnter Username of the user you want to delete : ";
	getline(cin >> ws, Username);

	if (IsUserFound(vUsers, Username, UserFound)) {

		PrintUserCard(UserFound);

		if (DeleteUserConfirmation()) {

			for (stUser& U : vUsers) {

				if (U.UserName == Username) {
					U.MarkToDelete = true;
				}

			}
			DeleteUserFromFile(vUsers);
			cout << "\nUser Deleted Successfully!" << endl;
		}

	}
	else {
		cout << "\nUser with Username [" << Username << "] Not Found!";
	}
}
void DeleteUserScreen() {

	cout << "\n----------------------------------\n";
	cout << "\tDelete Usres Screen";
	cout << "\n----------------------------------\n";
	vector<stUser>vUsers = LoadUsersFromFile(UsersFileName);
	DeleteUser(vUsers);

}


void SaveUsersDataToFile(vector<stUser>& vUsers) {

	fstream MyFile;

	MyFile.open(UsersFileName, ios::out);
	if (MyFile.is_open()) {

		for (stUser& U : vUsers) {

			MyFile << ConvertUserRecordToLine(U) << endl;
		}
		MyFile.close();
	}

}
void ChangeUserInfo(stUser& User) {

	cout << "\n Enter New Password : ";
	getline(cin >> ws, User.Password);

	char answer = ' ';

	cout << "\nDo you want to give this user full acess? y/n : ";
	cin >> answer;

	if (tolower(answer) == 'y') {
		User.Permissions += enMainMenuPermissions::eAll;
	}
	else {
		cout << "\nDo you want to give access to : ";

		cout << "\nShow Clients List? y/n : ";
		cin >> answer;
		if (tolower(answer) == 'y') {
			User.Permissions += enMainMenuPermissions::pClientList;
		}

		cout << "\nAdd New Client : ";
		cin >> answer;
		if (tolower(answer) == 'y') {
			User.Permissions += enMainMenuPermissions::pAddClient;

		}

		cout << "\nDelete Client : ";
		cin >> answer;
		if (tolower(answer) == 'y') {
			User.Permissions += enMainMenuPermissions::pDeleteClient;
		}

		cout << "\n Update Client : ";
		cin >> answer;
		if (tolower(answer) == 'y') {
			User.Permissions += enMainMenuPermissions::pUpdateClient;
		}

		cout << "\nFind Client : ";
		cin >> answer;
		if (tolower(answer) == 'y') {
			User.Permissions += enMainMenuPermissions::pFindClient;
		}

		cout << "\n Transactions: ";
		cin >> answer;
		if (tolower(answer) == 'y') {
			User.Permissions += enMainMenuPermissions::pTransactions;
		}

		cout << "\n Manage Users : ";
		cin >> answer;
		if (tolower(answer) == 'y') {
			User.Permissions += enMainMenuPermissions::pManageUsers;
		}

	}

}
bool UpdateUserConfirmation() {

	char answer = ' ';

	cout << "\nAre you sure you want to Update this User : ";
	cin >> answer;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	if (tolower(answer) == 'y')
	{
		return 1;
	}
	return 0;

}
void UpdateUser(vector<stUser>& vUsers) {

	string Username;
	stUser UserFound;

	cout << "\nEnter Username of the user you want to Update : ";
	getline(cin >> ws, Username);

	if (IsUserFound(vUsers, Username, UserFound)) {

		PrintUserCard(UserFound);

		if (UpdateUserConfirmation()) {

			for (stUser& U : vUsers) {

				if (U.UserName == Username) {
					ChangeUserInfo(U);
				}
			}
		}
		SaveUsersDataToFile(vUsers);
		cout << "\nUser Updated Successfully!" << endl;
	}
	else {
		cout << "\nUser with Username [" << Username << "] Not Found!";
	}


}
void UpdateUserInfoScreen() {

	cout << "\n----------------------------------\n";
	cout << "\tUpdate Usres Screen";
	cout << "\n----------------------------------\n";
	vector<stUser>vUsers = LoadUsersFromFile(UsersFileName);
	UpdateUser(vUsers);

}

void FindUser(vector<stUser>& vUsers) {

	stUser UserFound;
	string Username;

	cout << "\nEnter The username of the user you're looking for : ";
	getline(cin >> ws, Username);

	if (IsUserFound(vUsers, Username, UserFound)) {

		PrintUserCard(UserFound);
	}
	else {
		cout << "\nUser with username [" << Username << "] Not Found!";
	}
}
void FindUserScreen() {

	cout << "\n----------------------------------\n";
	cout << "\tFind Usres Screen";
	cout << "\n----------------------------------\n";
	vector<stUser>vUsers = LoadUsersFromFile(UsersFileName);
	FindUser(vUsers);

}

void GoBacKToMain() {
	cout << "Press any key to get back to the main Screen...";
	system("pause");
	MainMenuScreen();
}


void PerformTransactionOption(enTransactionOptions TransactionOption) {

	switch (TransactionOption) {
	case enTransactionOptions::eDeposite:
		system("cls");
		DepositeScreen();
		GoBacKToTransactionMenu();
		break;
		case enTransactionOptions::eWithdraw :
		system("cls");
		WithdrawScreen();
		GoBacKToTransactionMenu();
		break;
		case enTransactionOptions::eTotalBalance:
		system("cls");
		TotalBalancesScreen();
		GoBacKToTransactionMenu();
		break;
		case enTransactionOptions::eMainMenu:
		GoBacKToMain();
		break;
	}

}
void TransactionScreen() {

	if (!CheckAccessPermission(enMainMenuPermissions::pTransactions)) {
		ShowAccessDeniedScreen();
		GoBacKToMain();
		return;
	}

	system("cls");
	cout << "========================================\n";
	cout << "\t  Transactions Menu Screen\n";
	cout << "========================================\n";
	cout << "\t[1] Deposit.\n";
	cout << "\t[2] WithDraw.\n";
	cout << "\t[3] Total Balance.\n";
	cout << "\t[4] Main Menu.\n";
	cout << "========================================\n";

	PerformTransactionOption((enTransactionOptions)ChooseOption(1, 4));
}

void GoBackToManageUsers() {
	cout << "\n\nPress any key to get back to the Users Panel...";
	system("pause");
	ManageUsersScreen();


}
void PerformUserOption(enUserOption UserOption) {

	switch (UserOption) {
	case enUserOption::eUsersList:
		system("cls");
		ShowUsersList();
		GoBackToManageUsers();
		break;
	case enUserOption::eAddUser:
		system("cls");
		AddUsersScreen();
		GoBackToManageUsers();
		break;
	case enUserOption::eDeleteUser:
		system("cls");
		DeleteUserScreen();
		GoBackToManageUsers();
		break;
	case enUserOption::eUpdateUser:
		system("cls");
		UpdateUserInfoScreen();
		GoBackToManageUsers();
		break;
	case enUserOption::eFindUser:
		system("cls");
		FindUserScreen();
		GoBackToManageUsers();
		break;
	case enUserOption::eMenu:
		GoBacKToMain();
		break;
	}

}
void ManageUsersScreen() {

	if (!CheckAccessPermission(enMainMenuPermissions::pManageUsers)) {
		ShowAccessDeniedScreen();
		GoBacKToMain();
		return;
	}

	system("cls");
	cout << "========================================\n";
	cout << "\t Manage Users Menu Screen\n";
	cout << "========================================\n";
	cout << "\t[1] Users List.\n";
	cout << "\t[2] Add New User.\n";
	cout << "\t[3] Delete User.\n";
	cout << "\t[4] Update User.\n";
	cout << "\t[5] Find User.\n";
	cout << "\t[6] Main Menu.\n";
	cout << "========================================\n";

	PerformUserOption((enUserOption)ChooseOption(1, 6));

}

void GoBackToLoginScreen() {

	Login();
}
void PerformOption(enOptions Option) {

	switch (Option)
	{
	case eShowClients:
		system("cls");
		ShowClientsList();
		GoBacKToMain();
		break;
	case eAddClient:
		system("cls");
		AddClientScreen();
		GoBacKToMain();
		break;
	case eDeleteClient:
		system("cls");
		DeleteClientScreen();
		GoBacKToMain();
		break;
	case eUpdateClient:
		system("cls");
		UpdateClientScreen();
		GoBacKToMain();
		break;
	case eFindClient:
		system("cls");
		FindClientScreen();
		GoBacKToMain();
		break;
	case eTransactions:
		system("cls");
		TransactionScreen();
		break;
	case enOptions::eManageUsers:
		system("cls");
		ManageUsersScreen();
	case eLogOut:
		system("cls");
		GoBackToLoginScreen();
		break;
	}

}

void MainMenuScreen() {

	system("cls");
	cout << "========================================\n";
	cout << "\t  Main Menu Screen\n";
	cout << "========================================\n";
	cout << "\t[1] Show client List.\n";
	cout << "\t[2] Add New Client.\n";
	cout << "\t[3] Delete Client.\n";
	cout << "\t[4] Update Client Info.\n";
	cout << "\t[5] Find Client .\n";
	cout << "\t[6] Transactions.\n";
	cout << "\t[7] Manage Users.\n";
	cout << "\t[8] Log Out.\n";
	cout << "========================================\n";

	PerformOption((enOptions)ChooseOption(1, 8));

}
void Bank() {

	MainMenuScreen();

}


bool CheckCresidentials(string Username, string Password, stUser& User) {

	vector<stUser>vUsers = LoadUsersFromFile(UsersFileName);
	
	for (stUser U : vUsers) {

		if (U.UserName == Username && U.Password == Password) {
			User = U;
			return true;
		}
	}
	return false;
}
void LoginHeader() {

	system("cls");
	cout << "\n----------------------------------\n";
	cout << "\tLogin Screen";
	cout << "\n----------------------------------\n";


}
void Login() {

	system("cls");
	cout << "\n----------------------------------\n";
	cout << "\tLogin Screen";
	cout << "\n----------------------------------\n";

	string Username, Password;

	cout << "Enter Username : ";
	getline(cin >> ws, Username);
	cout << "Enter Password: ";
	getline(cin >> ws, Password);

	while(!CheckCresidentials(Username, Password, CurrentUser)) {

		LoginHeader();
		cout << "Invalid Username/Password\n";
		cout << "\nEnter Username : ";
		getline(cin >> ws, Username);
		cout << "Enter Password: ";
		getline(cin >> ws, Password);
	}

	Bank();

}


int main()
{
	Login();

	return 0;
}