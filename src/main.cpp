#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <functional>
#include <limits>
#include <filesystem>
#include <cctype>

struct Account
{
    int accountNumber{};
    std::string fullName;
    std::string pinHash;
    double balance{};
    std::vector<std::string> transactions;
};

std::filesystem::path getAccountFilePath()
{
#ifdef ACCOUNT_FILE_PATH
    return std::filesystem::path(ACCOUNT_FILE_PATH);
#else
    return std::filesystem::path("data") / "accounts.txt";
#endif
}

bool prepareAccountFile(const std::filesystem::path& filename)
{
    std::filesystem::path folder = filename.parent_path();

    if (!folder.empty())
    {
        std::error_code error;
        std::filesystem::create_directories(folder, error);

        if (error)
        {
            std::cout << "Error: Could not create data folder." << std::endl;
            std::cout << "Folder: " << folder.string() << std::endl;
            return false;
        }
    }

    std::ofstream file(filename, std::ios::app);

    if (!file)
    {
        std::cout << "Error: Could not open account file." << std::endl;
        std::cout << "File path: " << filename.string() << std::endl;
        return false;
    }

    return true;
}

std::string hashPin(const std::string& pin)
{
    std::hash<std::string> hasher;
    return std::to_string(hasher(pin));
}

bool isFourDigitPin(const std::string& pin)
{
    if (pin.length() != 4)
    {
        return false;
    }

    for (char ch : pin)
    {
        if (!std::isdigit(static_cast<unsigned char>(ch)))
        {
            return false;
        }
    }

    return true;
}

void displayMainMenu()
{
    std::cout << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "        Simple Banking System" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "1. Create Account" << std::endl;
    std::cout << "2. Login" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << "Please choose an option: ";
}

void displayAccountMenu()
{
    std::cout << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "          Account Menu" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "1. View Account Details" << std::endl;
    std::cout << "2. Check Balance" << std::endl;
    std::cout << "3. Deposit Money" << std::endl;
    std::cout << "4. Withdraw Money" << std::endl;
    std::cout << "5. Transfer Money" << std::endl;
    std::cout << "6. View Transaction History" << std::endl;
    std::cout << "7. Logout" << std::endl;
    std::cout << "Please choose an option: ";
}

bool readInteger(int& value)
{
    std::cin >> value;

    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a number." << std::endl;
        return false;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
}

bool readDouble(double& value)
{
    std::cin >> value;

    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid amount. Please enter a number." << std::endl;
        return false;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
}

int generateAccountNumber(const std::vector<Account>& accounts)
{
    int maxNumber = 1000;

    for (const Account& account : accounts)
    {
        if (account.accountNumber > maxNumber)
        {
            maxNumber = account.accountNumber;
        }
    }

    return maxNumber + 1;
}

int findAccountIndex(const std::vector<Account>& accounts, int accountNumber)
{
    for (int i = 0; i < static_cast<int>(accounts.size()); ++i)
    {
        if (accounts[i].accountNumber == accountNumber)
        {
            return i;
        }
    }

    return -1;
}

bool createAccount(std::vector<Account>& accounts, int& createdAccountNumber)
{
    Account newAccount;
    std::string pin;
    std::string confirmPin;

    newAccount.accountNumber = generateAccountNumber(accounts);

    std::cout << "Enter full name: ";
    std::getline(std::cin, newAccount.fullName);

    if (newAccount.fullName.empty())
    {
        std::cout << "Full name cannot be empty." << std::endl;
        return false;
    }

    std::cout << "Create a 4-digit PIN: ";
    std::getline(std::cin, pin);

    std::cout << "Confirm PIN: ";
    std::getline(std::cin, confirmPin);

    if (!isFourDigitPin(pin))
    {
        std::cout << "PIN must be exactly 4 digits." << std::endl;
        return false;
    }

    if (pin != confirmPin)
    {
        std::cout << "PINs do not match." << std::endl;
        return false;
    }

    newAccount.pinHash = hashPin(pin);
    newAccount.balance = 0.0;
    newAccount.transactions.push_back("Account created with balance 0.00");

    createdAccountNumber = newAccount.accountNumber;
    accounts.push_back(newAccount);

    return true;
}

int loginAccount(const std::vector<Account>& accounts)
{
    int accountNumber;
    std::string pin;

    std::cout << "Enter account number: ";

    if (!readInteger(accountNumber))
    {
        return -1;
    }

    std::cout << "Enter PIN: ";
    std::getline(std::cin, pin);

    std::string enteredHash = hashPin(pin);

    for (int i = 0; i < static_cast<int>(accounts.size()); ++i)
    {
        if (accounts[i].accountNumber == accountNumber &&
            accounts[i].pinHash == enteredHash)
        {
            std::cout << "Login successful. Welcome, "
                << accounts[i].fullName << "!" << std::endl;
            return i;
        }
    }

    std::cout << "Invalid account number or PIN." << std::endl;
    return -1;
}

void viewAccountDetails(const Account& account)
{
    std::cout << std::endl;
    std::cout << "========== Account Details ==========" << std::endl;
    std::cout << "Account Number: " << account.accountNumber << std::endl;
    std::cout << "Full Name     : " << account.fullName << std::endl;
    std::cout << "Balance       : "
        << std::fixed << std::setprecision(2)
        << account.balance << std::endl;
}

void checkBalance(const Account& account)
{
    std::cout << "Current balance: "
        << std::fixed << std::setprecision(2)
        << account.balance << std::endl;
}

bool depositMoney(Account& account)
{
    double amount;

    std::cout << "Enter amount to deposit: ";

    if (!readDouble(amount))
    {
        return false;
    }

    if (amount <= 0)
    {
        std::cout << "Deposit amount must be greater than zero." << std::endl;
        return false;
    }

    account.balance += amount;

    std::ostringstream oss;
    oss << "Deposited " << std::fixed << std::setprecision(2) << amount;
    account.transactions.push_back(oss.str());

    return true;
}

bool withdrawMoney(Account& account)
{
    double amount;

    std::cout << "Enter amount to withdraw: ";

    if (!readDouble(amount))
    {
        return false;
    }

    if (amount <= 0)
    {
        std::cout << "Withdrawal amount must be greater than zero." << std::endl;
        return false;
    }

    if (amount > account.balance)
    {
        std::cout << "Insufficient balance." << std::endl;
        return false;
    }

    account.balance -= amount;

    std::ostringstream oss;
    oss << "Withdrew " << std::fixed << std::setprecision(2) << amount;
    account.transactions.push_back(oss.str());

    return true;
}

bool transferMoney(std::vector<Account>& accounts, int senderIndex)
{
    int receiverAccountNumber;
    double amount;

    std::cout << "Enter receiver account number: ";

    if (!readInteger(receiverAccountNumber))
    {
        return false;
    }

    std::cout << "Enter amount to transfer: ";

    if (!readDouble(amount))
    {
        return false;
    }

    if (amount <= 0)
    {
        std::cout << "Transfer amount must be greater than zero." << std::endl;
        return false;
    }

    int receiverIndex = findAccountIndex(accounts, receiverAccountNumber);

    if (receiverIndex == -1)
    {
        std::cout << "Receiver account not found." << std::endl;
        return false;
    }

    if (receiverIndex == senderIndex)
    {
        std::cout << "You cannot transfer money to the same account." << std::endl;
        return false;
    }

    if (amount > accounts[senderIndex].balance)
    {
        std::cout << "Insufficient balance." << std::endl;
        return false;
    }

    accounts[senderIndex].balance -= amount;
    accounts[receiverIndex].balance += amount;

    std::ostringstream senderLog;
    senderLog << "Transferred " << std::fixed << std::setprecision(2)
        << amount << " to account " << receiverAccountNumber;

    std::ostringstream receiverLog;
    receiverLog << "Received " << std::fixed << std::setprecision(2)
        << amount << " from account "
        << accounts[senderIndex].accountNumber;

    accounts[senderIndex].transactions.push_back(senderLog.str());
    accounts[receiverIndex].transactions.push_back(receiverLog.str());

    return true;
}

void viewTransactionHistory(const Account& account)
{
    std::cout << std::endl;
    std::cout << "========== Transaction History ==========" << std::endl;

    if (account.transactions.empty())
    {
        std::cout << "No transactions found." << std::endl;
        return;
    }

    for (const std::string& transaction : account.transactions)
    {
        std::cout << "- " << transaction << std::endl;
    }
}

bool saveAccountsToFile(const std::vector<Account>& accounts,
    const std::filesystem::path& filename)
{
    if (!prepareAccountFile(filename))
    {
        return false;
    }

    std::ofstream file(filename);

    if (!file)
    {
        std::cout << "Error: Could not open account file for writing." << std::endl;
        std::cout << "File path: " << filename.string() << std::endl;
        return false;
    }

    for (const Account& account : accounts)
    {
        file << account.accountNumber << "|"
            << account.fullName << "|"
            << account.pinHash << "|"
            << std::fixed << std::setprecision(2) << account.balance << "|";

        for (int i = 0; i < static_cast<int>(account.transactions.size()); ++i)
        {
            file << account.transactions[i];

            if (i < static_cast<int>(account.transactions.size()) - 1)
            {
                file << ";";
            }
        }

        file << std::endl;
    }

    file.close();
    return true;
}

void loadAccountsFromFile(std::vector<Account>& accounts,
    const std::filesystem::path& filename)
{
    if (!prepareAccountFile(filename))
    {
        std::cout << "Starting with no accounts because the account file could not be prepared." << std::endl;
        return;
    }

    std::ifstream file(filename);

    if (!file)
    {
        std::cout << "No existing account file found. Starting with no accounts." << std::endl;
        return;
    }

    accounts.clear();

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        std::stringstream ss(line);
        std::string accountNumberText;
        std::string balanceText;
        std::string transactionText;
        Account account;

        std::getline(ss, accountNumberText, '|');
        std::getline(ss, account.fullName, '|');
        std::getline(ss, account.pinHash, '|');
        std::getline(ss, balanceText, '|');
        std::getline(ss, transactionText);

        try
        {
            account.accountNumber = std::stoi(accountNumberText);
            account.balance = std::stod(balanceText);
        }
        catch (...)
        {
            std::cout << "Warning: Skipped an invalid account record." << std::endl;
            continue;
        }

        std::stringstream transactionStream(transactionText);
        std::string transaction;

        while (std::getline(transactionStream, transaction, ';'))
        {
            if (!transaction.empty())
            {
                account.transactions.push_back(transaction);
            }
        }

        accounts.push_back(account);
    }

    file.close();

    std::cout << "Account records loaded successfully." << std::endl;
}

void accountArea(std::vector<Account>& accounts,
    int accountIndex,
    const std::filesystem::path& filename)
{
    int choice;

    while (true)
    {
        displayAccountMenu();

        if (!readInteger(choice))
        {
            continue;
        }

        switch (choice)
        {
        case 1:
            viewAccountDetails(accounts[accountIndex]);
            break;

        case 2:
            checkBalance(accounts[accountIndex]);
            break;

        case 3:
            if (depositMoney(accounts[accountIndex]))
            {
                if (saveAccountsToFile(accounts, filename))
                {
                    std::cout << "Deposit successful." << std::endl;
                }
                else
                {
                    std::cout << "Deposit was made in memory, but could not be saved to file." << std::endl;
                }
            }
            break;

        case 4:
            if (withdrawMoney(accounts[accountIndex]))
            {
                if (saveAccountsToFile(accounts, filename))
                {
                    std::cout << "Withdrawal successful." << std::endl;
                }
                else
                {
                    std::cout << "Withdrawal was made in memory, but could not be saved to file." << std::endl;
                }
            }
            break;

        case 5:
            if (transferMoney(accounts, accountIndex))
            {
                if (saveAccountsToFile(accounts, filename))
                {
                    std::cout << "Transfer successful." << std::endl;
                }
                else
                {
                    std::cout << "Transfer was made in memory, but could not be saved to file." << std::endl;
                }
            }
            break;

        case 6:
            viewTransactionHistory(accounts[accountIndex]);
            break;

        case 7:
            std::cout << "You have logged out." << std::endl;
            return;

        default:
            std::cout << "Invalid option. Please choose again." << std::endl;
        }
    }
}

int main()
{
    std::vector<Account> accounts;
    std::filesystem::path filename = getAccountFilePath();
    int choice;

    loadAccountsFromFile(accounts, filename);

    while (true)
    {
        displayMainMenu();

        if (!readInteger(choice))
        {
            continue;
        }

        switch (choice)
        {
        case 1:
        {
            int createdAccountNumber = 0;

            if (createAccount(accounts, createdAccountNumber))
            {
                if (saveAccountsToFile(accounts, filename))
                {
                    std::cout << "Account created successfully." << std::endl;
                    std::cout << "Your account number is: " << createdAccountNumber << std::endl;
                }
                else
                {
                    accounts.pop_back();
                    std::cout << "Account was not created because it could not be saved to file." << std::endl;
                }
            }

            break;
        }

        case 2:
        {
            int accountIndex = loginAccount(accounts);

            if (accountIndex != -1)
            {
                accountArea(accounts, accountIndex, filename);
            }

            break;
        }

        case 3:
            saveAccountsToFile(accounts, filename);
            std::cout << "Thank you for using the Simple Banking System." << std::endl;
            return 0;

        default:
            std::cout << "Invalid option. Please choose again." << std::endl;
        }
    }
}