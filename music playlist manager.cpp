#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <conio.h>
#include <windows.h>
using namespace std;

struct Node
{
    bool isFavorite = false;
    char song[100] = {};
    Node *next = nullptr;
    Node *prev = nullptr;
};

void addsongtofile(const char a[]);
void addNode(Node *first);
void markFavorite(Node *first);
void displayFavorites(const Node *first);
void shufflePlay(Node *first);
void addNodeFromFile(Node *first, const string& a);
void deleteFileSong(const char a[]);
void deleteNode(Node *&first, int pos);
void printList(const Node *first);
void searchSong(Node *first);
void sortPlaylist(Node *&head);
void createRecentlyPlayed();
void pushToRecentlyPlayed(const char data[]);
void displayRecentlyPlayed();
void playSong(Node *first);
void showLastPlayedSong();
void cleanMemory(Node *head);
void clearTempFile();

void initializePlaylist(Node *&head);
string getHiddenPassword();
bool registerUser();
bool loginUser();
void loadUserPlaylist(Node *first);

string currentUsername;

//save song to user's playlist txt file
void addsongtofile(const char a[]) {
    if (currentUsername.empty()) {
        cout << "Error: No user logged in.\n";
        return;
    }
    string filename = currentUsername + "_playlist.txt";
    fstream f1(filename, ios::out | ios::app);
    if (!f1) {
        cout << "Error: Unable to open playlist file.\n";
        return;
    }
    f1 << a << endl;
}

//load songs from user's playlist file on login
void loadUserPlaylist(Node *first) {
    if (currentUsername.empty()) {
        cout << "Error: No user logged in.\n";
        return;
    }
    string filename = currentUsername + "_playlist.txt";
    fstream f1(filename, ios::in);

    string songName;
    while (getline(f1, songName)) {
        addNodeFromFile(first, songName);
    }
    f1.close();
}
string getHiddenPassword()
{
    string password;
    char ch;

    while ((ch = getch()) != '\r') {
        if (ch == '\b') {
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        } else {
            password += ch;
            cout << '*';
        }
    }
    cout << endl;
    return password;
}


bool registerUser() {
    cout << "Enter a username: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, currentUsername);
    cout << "Enter a password: ";
    string password = getHiddenPassword();

    ofstream file("users.txt", ios::app);
    if (file) {
        file << currentUsername << "\n" << password << "\n";
        cout<< endl;
        cout << "Registration successful!\n";
        return true;
    } else {
        cout << "Error: Unable to open user data file.\n";
        return false;
    }
}


bool loginUser()
{
    cout << "Enter username: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, currentUsername);

    cout << "Enter password: ";
    string inputPassword = getHiddenPassword();

    ifstream file("users.txt");
    if (file) {
        string fileUsername, filePassword;
        while (getline(file, fileUsername) && getline(file, filePassword)) {
            if (fileUsername == currentUsername && filePassword ==
inputPassword) {
                cout<< endl;
                cout << "Login successful!\n";
                return true;
            }
        }
        cout << "Incorrect username or password.\n";
        return false;
    } else {
        cout << "Error: Unable to open user data file.\n";
        return false;
    }
}

Node *top = nullptr;

// Adding a new node at the end of the playlist
void addNode(Node *first) {
    Node *current = first;
    while (current->next != nullptr) {
        current = current->next;
    }

    char song[100];
    cout << "\nEnter Song name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.getline(song, 100);
    Node *temp = first;
    bool found = false;
    while (temp != nullptr) {
        if (strcmp(temp->song, song) == 0) {
            cout << "This song is already in the playlist.\n";
            found = true;
            break;
        }
        temp = temp->next;
    }

    if (!found) {
        current->next = new Node;
        current->next->prev = current;

        current = current->next;

        strcpy(current->song, song);
        addsongtofile(current->song);
        cout << "Song added successfully!";
        cout<< endl;
    }
}

void markFavorite(Node *first) {
    char song[100];

    cout << "Enter the song name to mark as favorite: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.getline(song, 100);

    bool found = false;

    while (first != nullptr) {
        if (strcmp(first->song, song) == 0) {
            first->isFavorite = true;
            cout << "Song marked as favorite!\n";
            found = true;
            break;
        }
        first = first->next;
    }

    if (!found) {
        cout << "Song not found.\n";
    }
}


void displayFavorites(const Node *first) {
    cout << "\nFavorite Songs:\n";
    bool hasFavorites = false;
    while (first != nullptr) {
        if (first->isFavorite) {
            cout << "- " << first->song << endl;
            hasFavorites = true;
        }
        first = first->next;
    }
    if (!hasFavorites) {
        cout << "No favorite songs found.\n";
    }
}

void shufflePlay(Node *first) {
    srand(static_cast<unsigned int>(time(0)));
    int songCount = 0;
    Node *temp = first;


    while (temp != nullptr) {
        songCount++;
        temp = temp->next;
    }

    if (songCount == 0) {
        cout << "No songs available to play.\n"; // No songs to play
        return;
    }
    int randomIndex = rand() % songCount;
    temp = first;

    for (int i = 0; i < randomIndex; i++) {
        temp = temp->next;
    }

    cout << "\nNow Playing (Shuffle): " << temp->song << endl;

    pushToRecentlyPlayed(temp->song);
}

// delete a song from user's txt file
void deleteFileSong(const char a[]) {
    if (currentUsername.empty()) {
        cout << "Error: No user is logged in.\n";
        return;
    }

    string filename = currentUsername + "_playlist.txt";
    fstream f1(filename, ios::in);
    fstream f2("temp.txt", ios::out);
    string line;
    bool found = false;

    if (!f1) {
        cout << "Error: Unable to open playlist file for " << currentUsername <<
".\n";
        return;
    }

    while (getline(f1, line)) {
        if (strcmp(a, line.c_str()) != 0) {
            f2 << line << endl;
        } else {
            found = true;
        }
    }
    f1.close();
    f2.close();

    if (found) {
        if (remove(filename.c_str()) != 0) {
            cout << "Error: Unable to delete the original playlist file.\n";
        } else if (rename("temp.txt", filename.c_str()) != 0) {
            cout << "Error: Unable to rename temp file to playlist file.\n";
        } else {
            cout << "Song has been deleted from the playlist.\n";
        }
    } else {
        remove("temp.txt");
        cout << "Song not found in the playlist.\n";
    }
}


// Deleting a node from the linked list
void deleteNode(Node *&first, int pos) {
    Node *temp = first;

    if (pos == 0) {
        first = first->next;
        if (first != nullptr) {
            first->prev = nullptr;
        }
        delete temp;
        return;
    }

    for (int i = 0; temp != nullptr && i < pos; i++) {
        temp = temp->next;
    }

    if (temp == nullptr) return;

    if (temp->prev != nullptr) {
        temp->prev->next = temp->next;
    }
    if (temp->next != nullptr) {
        temp->next->prev = temp->prev;
    }

    delete temp;
}


void printList(const Node *first) {
    cout << "\nPlaylist:\n";
    while (first != nullptr) {
        cout << "- " << first->song << endl;
        first = first->next;
    }
}


void searchSong(Node *first) {
    char song[100];
    cout << "\nEnter song name to search: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.getline(song, 100);

    bool found = false;

    while (first != nullptr) {
        if (strcmp(first->song, song) == 0) {
            cout << "\n Song Found: " << first->song << endl;
            found = true;
            break;
        }
        first = first->next;
    }
    if (!found) {
        cout << "Song not found in playlist.\n";
    }
}


void sortPlaylist(Node *&head)
{
    if (head == nullptr) return;

    bool swapped;
    do {
        swapped = false;
        Node *current = head;

        while (current->next != nullptr) {
            if (strcmp(current->song, current->next->song) > 0) {
                char temp[100];
                strcpy(temp, current->song);
                strcpy(current->song, current->next->song);
                strcpy(current->next->song, temp);
                swapped = true;
            }
            current = current->next;
        }
    } while (swapped);
}


//add a song to the playlist from txt file
void addNodeFromFile(Node *first, const string& a) {
    while (first->next != nullptr) {
        first = first->next;
    }
    first->next = new Node;
    first->next->prev = first;
    first = first->next;

    strcpy(first->song, a.c_str());
}

//delete a song from the playlist
void deleteSongMenu(Node *&first) {
    printList(first);
    int pos;
    cout << "Enter the position of the song to delete: ";
    cin >> pos;
    Node *temp = first;
    int index = 0;
    while (temp != nullptr && index < pos) {
        temp = temp->next;
        index++;
    }

    if (temp == nullptr) {
        cout << "Invalid position!\n";
        return;
    }

    char songToDelete[100];
    strcpy(songToDelete, temp->song);

    deleteNode(first, pos);

    deleteFileSong(songToDelete);
}

void createRecentlyPlayed() {
    top = nullptr;
}

// Push song to the recently played stack
void pushToRecentlyPlayed(const char data[]) {
    Node *newNode = new Node;
    strcpy(newNode->song, data);
    newNode->next = top;
    top = newNode;
}


void displayRecentlyPlayed() {
    if (top == nullptr) {
        cout << "No recently played songs.\n";
        return;
    }
    cout << "\nRecently Played:\n";
    Node *temp = top;
    while (temp != nullptr) {
        cout << "- " << temp->song << endl;
        temp = temp->next;
    }
}


void playSong(Node *first) {
    char song[100];
    cout << "\nEnter the song to play: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.getline(song, 100);
    Node *temp = first;

    while (temp != nullptr) {
        if (strcmp(temp->song, song) == 0) {
            cout << "Playing: " << temp->song << endl;
            pushToRecentlyPlayed(temp->song);
            return;
        }
        temp = temp->next;
    }
    cout << "Song not found in playlist.\n";
}



void showLastPlayedSong() {
    if (top != nullptr) {
        cout << "Last played song: " << top->song << endl;
    } else {
        cout << "No songs have been played yet.\n";
    }
}


void cleanMemory(Node *head) {
    Node *temp;
    while (head != nullptr) {
        temp = head;
        head = head->next;
        delete temp;
    }
    while (top != nullptr) {
        temp = top;
        top = top->next;
        delete temp;
    }
}


void clearTempFile() {
    remove("temp.txt");
}

void initializePlaylist(Node *&head) {
    head = new Node;
    head->next = nullptr;
}

// text and background colors
void setConsoleColor(int textColor, int bgColor) {
    int color = (bgColor << 4) | textColor;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// background color of the console window
void setWindowBackgroundColor(int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwSize;
    COORD coord = {0, 0};
    DWORD dwWritten;

    GetConsoleScreenBufferInfo(hConsole, &csbi);

    dwSize = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputAttribute(hConsole, bgColor << 4, dwSize, coord,
&dwWritten);
    FillConsoleOutputCharacter(hConsole, ' ', dwSize, coord, &dwWritten);

    SetConsoleCursorPosition(hConsole, coord);
}

int main() {
    setWindowBackgroundColor(7);
    setConsoleColor(0, 7);
    int loginChoice;
    bool loggedIn = false;

    while (!loggedIn) {
        cout<< endl;
        cout<<"                                                ||WELCOME TO MUSIC PLAYLIST MANAGER||"<<endl;
        cout<< endl;
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout<< endl;
        cout << "Enter your choice: ";
        cin >> loginChoice;

        if (loginChoice == 1)
        {
            loggedIn = registerUser();
        }
        else if (loginChoice == 2)
        {
            loggedIn = loginUser();
        }
        else
        {
            cout << "Invalid choice.\n";
        }
    }

    if (loggedIn)
    {
        Node *head = nullptr;
        initializePlaylist(head);
        loadUserPlaylist(head);
        createRecentlyPlayed();

    int choice;

    do {

    cout<< endl;
    cout <<
"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Music Playlist Manager~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    cout<< endl;
    cout << "1. Add Song\n2. Mark Favorite\n3. Display Favorites\n4. Shuffle Play\n5. Delete Song\n6. Search Song\n7. Sort Playlist\n8. Display Playlist\n9. Display Recently Played\n10. Play Song\n11. Show Last Played Song\n12. Exit\n"; // Added option 10
    cout<< endl;
    cout << "Enter your choice: ";
    cin >> choice;

    switch (choice) {
        case 1:
            addNode(head);
            break;
        case 2:
            markFavorite(head);
            break;
        case 3:
            displayFavorites(head);
            break;
        case 4:
            shufflePlay(head);
            break;
        case 5:
            deleteSongMenu(head);
            break;
        case 6:
            searchSong(head);
            break;
        case 7:
            sortPlaylist(head);
            cout << "Playlist sorted successfully.\n";
            break;
        case 8:
            printList(head);
            break;
        case 9:
            displayRecentlyPlayed();
            break;
        case 10:
            playSong(head);
            break;
        case 11:
            showLastPlayedSong();
            break;
        case 12:
            cout<< "Exiting the program!"<< endl;
            return 0;
        default:
            cout << "Invalid choice! Please try again.\n";
            break;
    }

} while (choice != 12);

    cleanMemory(head);
    clearTempFile();
    }
    return 0;

}
