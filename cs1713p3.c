    /******************************************************************************
cs1713p3.c by Kenneth F. Schmidtling
Purpose:
    The program reads inventory information and a command file. It
    processes the commands against the inventory.

    This file contains functions:
        getInventory - reads and returns the inventory and the count of inventory
                       (most of this is in program #2)
        printInventory - prints the inventory  (most of this is in program #2)
        sortInventory - sorts the inventory using a bubble sort
        searchInventory - find a Stock Number in the sorted Inventory array using
                       a binary search algorithm.   You will use this in your routines.
        processOrderCommand - processes the various ORDER commands
                       (most of this is in program #2)
        processInventoryCommand - processes the various INVENTORY commands.
Input:
    Inventory   Stream input file which contains many records of stock inventory:
                szStockNumber   lStockQty   dUnitPrice   szStockName
                6s              8ld         10.2f        30s (may contain spaces)
    Command	    This is different from the previous assignment.  The file contains
                data in the form of commands (one command per data line):
                ORDER BEGIN email,full name
                    specifies the beginning of an order.  It also has the email
                    and full name of the customer (separated by commas)
                ORDER ADDRESS szStreetAddress,szCity,szStateCd,szZipCd
                    specifies the address for an order (separated by commas)
                ORDER ITEM szStockNumber,iRequestQty
                    specifies a purchase item.  The information will be separated by commas.
                    Lookup the stock number.  If not found, print an error.  If found, try to
                    satisfy the enire quantity with the inventory.  If not enough in stock,
                    show a message that the entire quantity is backordered.
                    If it was satisifed, show the unit price and its total cost.
                ORDER COMPLETE
                    specifies the completion of an order (i.e., marks the end of the purchase items).
                    Print the totals for the order.
                INVENTORY RECEIVE szStockNumber,quantity
                    increase the inventory quantity for the specified inventory item
                INVENTORY SHOW szStockNumber
                    requests a display of an inventory item.  Show all of its information.
Results:
    Prints the Inventory prior to sorting
    Prints the Inventory after sorting.
    Processes the commands (see above) and shows any errors.
    Prints the resulting Inventory
Returns:
    0  normal
    -1 invalid command line syntax
    -2 show usage only
    -3 error during processing, see stderr for more information

Notes:
    p3 -?       will provide the usage information.  In some shells,
                you will have to type p3 -\?
******************************************************************************/
// If compiling using visual studio, tell the compiler not to give its warnings
// about the safety of scanf and printf
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cs1713p3.h"

/******************** getInventory *******************************************
    int getInventory(Stock stockM[], char * pszInventoryFileName)
Purpose:
    Get the inventory from the data file and assign it into an array of Stocks.
Parameters:
    I   Stock stockM[]                  // Our Array of Stock
    I   char * pszInventoryFileName     // The pointer to the Inventory File
Returns:
    O   iStockCnt                       // The amount of items in the array
Notes:

******************************************************************************/
int getInventory(Stock stockM[], char * pszInventoryFileName)
{
    char szInputBuffer[100];		    // input buffer for reading data
    int i = 0;                          // subscript in stockM
    int iScanfCnt;                      // returned by sscanf
    FILE *pfileInventory;               // stream Input for Inventory data.
    int iStockCnt = 0;                  // amount of items in the stock array
    Stock *pStock = stockM;             // pointer to our array of stock


    /* open the Inventory stream data file */
    if (pszInventoryFileName == NULL)
        exitError(ERR_MISSING_SWITCH, "-i");

    // tries to open the inventory file, if wrong ERROR
    pfileInventory = fopen(pszInventoryFileName, "r");
    if (pfileInventory == NULL)
        exitError(ERR_INVENTORY_FILENAME, pszInventoryFileName);

    // while there is still data in the inventory file
    while (fgets(szInputBuffer, 100, pfileInventory) != NULL)
    {
        iScanfCnt = sscanf(szInputBuffer, "%7s %ld %lf %[^\n]\n"
                            , pStock[i].szStockNumber
                            , &pStock[i].lStockQty
                            , &pStock[i].dUnitPrice
                            , pStock[i].szStockName);

        // error checking
        if (iScanfCnt != 4)
            fprintf(stderr, "%s: %s\n", ERR_INVALID_INVENTORY_DATA, szInputBuffer);

        // if data is valid
        else if (iScanfCnt == 4)
        {
            iStockCnt++;
            i++;
        }
    }
        /*Check to see if our amount of stock items is larger than the max we set. */
        if (iStockCnt > MAX_STOCK)
            exitError(ERR_TOO_MANY_STOCK_ITEMS, pszInventoryFileName);
        else
            return iStockCnt;

}
/******************** sortInventory ******************************************
    void sortInventory(Stock stockM[], int iStockCnt) Purpose:     Sort the Stock
        Number of the Arrays using strcmp so that we can perform a binary search on them
        in the most efficent way possible.
Parameters:
    I   Stock stockM[]      The array that we will be sorting
    I   int iStockCnt       The size of array stockM, so that we can know
                                the   size of the array.
Returns:
    Does not return anything.
Notes:

******************************************************************************/
void sortInventory(Stock stockM[], int iStockCnt)
{
    int i, j;
    int bChange = TRUE;
    Stock tempStock;

    for (i = 0; i < iStockCnt - 1; i = i + 1)
    {
        bChange = FALSE;
        for (j = 0; j < (iStockCnt - i - 1); j++)
        {
            if (strcmp(stockM[i].szStockNumber, stockM[j].szStockNumber) < 0)
                {
                    tempStock = stockM[j + 1];
                    stockM[j + 1] = stockM[j];
                    stockM[j] = tempStock;
                    bChange = TRUE;
                }
        }
        if (! bChange)
            break;
    }
}

/******************** printInventory ******************************************
    void printInventory(char *pszHeading, Stock stockM[], int iStockCnt)
Purpose:
    The purpose of this function is to print the contents of the inventory list
    updating any changes that are made by the program.
Parameters:
    *pszHeading
    Stock stockM[]
    int iStockCnt
Returns:
    Nothing, just prints out the contents of the stock array.
Notes:
    None.
******************************************************************************/
void printInventory(char *pszHeading, Stock stockM[], int iStockCnt)
{
    int i;
    printf("%s\n", pszHeading);
    printf("    %-10s  %-30s %-8s %-10s\n"
           , "Stock #", "Stock Name"
           , "   Count", "Unit Price");

    for (i = 0; i < iStockCnt; i++)
    {
        printf("    %-10s  %-30s %8ld %10.2lf\n"
            , stockM[i].szStockNumber
            , stockM[i].szStockName
            , stockM[i].lStockQty
            , stockM[i].dUnitPrice);
    }
}

/********************processOrderCommand **************************************
    void processOrderCommand(Stock stockM[], int iStockCnt
     , char *pszSubCommand, char *pszRemainingInput
     , Customer *pCustomer, double *pdOrderTotalCost)
Purpose:
    Processes the subcommands associated with the ORDER command:
        ORDER BEGIN email,full name
              specifies the beginning of an order.  It also has the email
              and full name of the customer (separated by commas)
        ORDER ADDRESS szStreetAddress,szCity,szStateCd,szZipCd
              specifies the address for an order (separated by commas)
        ORDER ITEM szStockNumber,iRequestQty
              specifies a purchase item.  The information will be separated by commas.
              Lookup the stock number.  If not found, print an error.  If found, try to
              satisfy the enire quantity with the inventory.  If not enough in stock,
              show a message that the entire quantity is backordered.
              If it was satisifed, show the unit price and its total cost.
        ORDER COMPLETE
              specifies the completion of an order (i.e., marks the end of the purchase items).
              Print the totals for the order.
Parameters:
    I/O Stock stockM[]                Array of stock
    I   int   iStockCnt               Number of elments in stockM[]
    I   char  *pszSubCommand          Should be BEGIN, ADDRESS, ITEM or COMPLETE
    I   char  *pzRemainingInput       Points to the remaining characters in the input
                                      line (i.e., the characters that following the
                                      subcommand).
    I/O Customer *pCustomer           The BEGIN subcommand begins a new order.  The
                                      customer's Order Total Cost must be set to 0.
    I/O double   *pdOrderTotalCost    The order total cost.  This changes depending
                                      on the subcommand:
                                          BEGIN - set to 0
                                          ITEM - add the cost  (unless there is a warning)
Notes:

******************************************************************************/
void processOrderCommand(Stock stockM[], int iStockCnt
     , char *pszSubCommand, char *pszRemainingInput
     , Customer *pCustomer, double *pdOrderTotalCost)
{
    int iScanfCnt;
    PurchaseItem purchaseItem;
    char szInputBuffer[100];
    // BEGIN subcommand
    if (strcmp(pszSubCommand, "BEGIN") == 0)
    {
        *pdOrderTotalCost = 0;
        // get the Customer Identification Information
        iScanfCnt = sscanf(pszRemainingInput,"%[^,],%[^\n]"
                            , pCustomer->szEmailAddr
                            , pCustomer->szFullName);
        // error check
        if (iScanfCnt != 2)
            exitError(ERR_ORDER_CUSTOMER_INFO, pszRemainingInput);
    }
    // COMPLETE subcommand
    else if (strcmp(pszSubCommand, "COMPLETE") == 0)
    {
        // print the order's total cost
        printf("Order for: \nEMAIL:%-50s\nNAME:%-30s\nOrder Total: $%-8.2lf\n\n"
               , pCustomer->szEmailAddr
               , pCustomer->szFullName
               , *pdOrderTotalCost);
    }
    // ADDRESS subcommand
    else if (strcmp(pszSubCommand, "ADDRESS") == 0)
    {
        // get the postal address
        iScanfCnt = sscanf(pszRemainingInput,"%30s %20s %2s %5s"
                           , pCustomer->szStreetAddress
                           , pCustomer->szCity
                           , pCustomer->szStateCd
                           , pCustomer->szZipCd);
        // error check
        if (iScanfCnt != 4)
            exitError(ERR_ORDER_ADDRESS_DATA, pszRemainingInput);
    }
    // ITEM subcommand
    else if (strcmp(pszSubCommand, "ITEM") == 0)
    {
        double dItemTotal;
        int i;
        int j;
        dItemTotal = 0;
        // get a purchase item
        iScanfCnt = sscanf(pszRemainingInput, "%6s,%d\n"
                           , purchaseItem.szStockNumber
                           , &purchaseItem.iRequestQty);
        // error check
        if (iScanfCnt != 2)
                exitError(ERR_ORDER_ITEM_DATA, pszRemainingInput);
        // search through the stock list to see if the item exists
        j = search(stockM, iStockCnt, purchaseItem.szStockNumber);
        // check to see if the order can be filled, if not ERROR
        if (j != -1)
        {
            // if there is not enough inventory to fill the order
            if (stockM[j].lStockQty < purchaseItem.iRequestQty)
                {
                printf("%s\n", WARN_NOT_ENOUGH_IN_STOCK);
                printf("Sorry, the amount of %s that you requested in not in stock.\n There are %d is on backorder right now.\n", stockM[j].szStockNumber, purchaseItem.iRequestQty);
                }
            // if there is enough of the stock to fill the item
            else if (stockM[j].lStockQty >= purchaseItem.iRequestQty)
            {
                dItemTotal = stockM[j].dUnitPrice * purchaseItem.iRequestQty;
                printf("STOCK: %s QTY: %d, PRICE: $%6.2lf\n",purchaseItem.szStockNumber, purchaseItem.iRequestQty,dItemTotal);
                // change the element of the array to reflect the order
                stockM[j].lStockQty -= purchaseItem.iRequestQty;
                *pdOrderTotalCost += stockM[j].dUnitPrice * purchaseItem.iRequestQty;
            }
        }
    }
    // the subcommand for ORDER was not found
    else
        printf("invalid subcommand for ORDER\n");
}
/********************processInventoryCommand **********************************
    void processInventoryCommand(Stock stockM[], int iStockCnt
         , char *pszSubCommand, char *pszRemainingInput)
Purpose:
    Processes the subcommands associated with the INVENTORY command:
        INVENTORY RECEIVE szStockNumber,quantity
            increase the inventory quantity for the specified inventory item
        INVENTORY SHOW szStockNumber
            requests a display of an inventory item.  Show all of its information.

Parameters:
    I/O Stock stockM[]                Array of stock
    I   int   iStockCnt               Number of elments in stockM[]
    I   char  *pszSubCommand          Should be RECEIVE or SHOW
    I   char  *pzRemainingInput       Points to the remaining characters in the input
                                      line (i.e., the characters that following the
                                      subcommand).
Notes:

******************************************************************************/
void processInventoryCommand(Stock stockM[], int iStockCnt
                             , char *pszSubCommand, char *pszRemainingInput)
{
    Stock inventoryItem;
    int iScanfCnt;
    int i;
    // check to see what the subcommand is
    // if RECEIVE
    if (strcmp(pszSubCommand, "RECEIVE") == 0)
    {
        iScanfCnt = sscanf(pszRemainingInput, "%6s,%ld\n"
                           , inventoryItem.szStockNumber
                           , &inventoryItem.lStockQty);
        // error check
        if (iScanfCnt != 2)
            printf("%s\n", ERR_INVENTORY_RECEIVE);
        // if the input is valid
        else if (iScanfCnt = 2)
        {
            // check to make sure that the item is in the stock list
            i = search(stockM, iStockCnt, inventoryItem.szStockNumber);
            // add the recieved amount to the inventory item
            if (i >= 0)
                stockM[i].lStockQty += inventoryItem.lStockQty;
        }
    }
    // if SHOW
    else if (strcmp(pszSubCommand, "SHOW") == 0)
    {
        // check to see if there is valid information to read
        iScanfCnt = sscanf(pszRemainingInput, "%6s \n"
                           , inventoryItem.szStockNumber);
        // error check
        if (iScanfCnt != 1)
            printf("%s/n",ERR_INVENTORY_SHOW);
        // data read was valid
        else if(iScanfCnt = 1)
        {
        // find the inventory item's array location
        i = search(stockM, iStockCnt, inventoryItem.szStockNumber);
        printf("NAME:%s QUANTITY:%d PPU:$%.2lf\n\n"
               ,stockM[i].szStockNumber
               ,stockM[i].lStockQty
               ,stockM[i].dUnitPrice);
        }
    }
}
/******************** search **************************************************
    int search(Stock stockM[], int iStockCnt, char *pszMatchStockNumber)
Purpose:
    Use binary value of the strings so that we can quickly search through
    array of stock that we have in the program.
Parameters:
    I   Stock stockM[]                Array of stock
    I   int   iStockCnt               Number of elments in stockM[]
    I   char *pszMatchStockNumber     Stock number to find in the array
Returns:
    >= 0   subscript of where the match value was found
    -1     not found
Notes:

******************************************************************************/
int search(Stock stockM[], int iStockCnt, char *pszMatchStockNumber)
{
    int i;
    i = 0;
    // while the string has not been found and it is not 1 less than the total
    // stock count
    while (strcmp(pszMatchStockNumber, stockM[i].szStockNumber) != 0
           || i != iStockCnt - 1)
    {
        if (strcmp(pszMatchStockNumber, stockM[i].szStockNumber) != 0)
            i++;

        else if (strcmp(pszMatchStockNumber, stockM[i].szStockNumber) == 0)
        {
            //printf("Stock spot %d\n",i);
            return i;
        }

        if (i == iStockCnt)
        {
            printf("Sorry the stock was not found.\n");
            return -1;
        }
    }
}
