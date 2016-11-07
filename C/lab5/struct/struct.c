#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct contact {
    char name[30];
    int phone_number;
    char address[30];
};

//update_contact goes here
void update_contact(struct contact *contacts,char* name,int number,char* address){
    // note that you can't use contacts->name = name since the address of an array
    // is already fixed
    strcpy(contacts->name,name);
    (*contacts).phone_number = number;
    strcpy(contacts->address,address);
}


int main(int argc, char* argv[]) {
    struct contact friend;
    // set initial values for friend
    strcpy(friend.name, "Jane Doe");
    friend.phone_number = 377177377;
    strcpy(friend.address, "3771 University Avenue");

    char *name = "shan";
    int number = 1002257988;
    char *address = "Bay";
    // in hidden code set values for these 3 variables
    
    // call update_contact to change the values in friend
    update_contact(&friend, name, number, address);
    return 0;
}
