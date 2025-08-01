#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <time.h>

#define URL_LEN 100
#define CONTENT_LEN 500
#define HASH_SIZE 100
#define MAX_TABS 10
#define MAX_SUGGESTIONS 5

// ANSI color codes
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Theme definitions
int theme = 0; // 0: Default, 1: Dark, 2: Light
const char* HEADER_COLOR[] = { ANSI_COLOR_CYAN, ANSI_COLOR_BLUE, ANSI_COLOR_GREEN };
const char* TEXT_COLOR[] = { ANSI_COLOR_YELLOW, ANSI_COLOR_CYAN, ANSI_COLOR_MAGENTA };
const char* MENU_COLOR[] = { ANSI_COLOR_BLUE, ANSI_COLOR_MAGENTA, ANSI_COLOR_YELLOW };

// Singly Linked List for History
typedef struct HistoryNode {
    char url[URL_LEN];
    struct HistoryNode* next;
} HistoryNode;

typedef struct {
    HistoryNode* head;
    HistoryNode* current;
    int size;
} HistoryList;

// Circular Doubly Linked List for Tabs
typedef struct TabNode {
    char url[URL_LEN];
    struct TabNode* next;
    struct TabNode* prev;
} TabNode;

typedef struct {
    TabNode* current;
    int tab_count;
} TabList;

// Hash Table for URL Caching
typedef struct CacheNode {
    char url[URL_LEN];
    char content[CONTENT_LEN];
    struct CacheNode* next;
} CacheNode;

typedef struct {
    CacheNode* table[HASH_SIZE];
} HashTable;

// Stack for Backtracking
typedef struct StackNode {
    char url[URL_LEN];
    struct StackNode* next;
} StackNode;

typedef struct {
    StackNode* top;
} BackStack;

// Binary Search Tree for Bookmarks
typedef struct BookmarkNode {
    char url[URL_LEN];
    struct BookmarkNode* left;
    struct BookmarkNode* right;
} BookmarkNode;

// Browser selection (0: Default, 1: Edge, 2: Chrome)
int browser_choice = 0;

// Function Prototypes
void clearScreen();
void showHeader();
void showFakeFeatures();
void initHistory(HistoryList* history);
void addToHistory(HistoryList* history, const char* url);
void showHistory(HistoryList* history);
void clearHistory(HistoryList* history);
void searchHistory(HistoryList* history, TabList* tabs, HashTable* cache, BackStack* stack);
void initTabs(TabList* tabs);
void addTab(TabList* tabs, const char* url);
void switchTab(TabList* tabs, int index);
void showTabs(TabList* tabs);
void initHashTable(HashTable* cache);
unsigned int hash(const char* url);
void addToCache(HashTable* cache, const char* url, const char* content);
char* getFromCache(HashTable* cache, const char* url);
void initBackStack(BackStack* stack);
void pushBackStack(BackStack* stack, const char* url);
char* popBackStack(BackStack* stack);
void openURLInBrowser(const char* url);
void fetchContent(HashTable* cache, const char* url);
void showMenu();
int validateURL(const char* url);
void suggestURLs(HistoryList* history, const char* prefix);
void enterURL(HistoryList* history, TabList* tabs, HashTable* cache, BackStack* stack);
void goBack(HistoryList* history, TabList* tabs, BackStack* stack);
void goHome(HistoryList* history, TabList* tabs, HashTable* cache, BackStack* stack);
void setBrowser();
void changeTheme();
BookmarkNode* initBookmarks();
BookmarkNode* insertBookmark(BookmarkNode* root, const char* url);
int bookmarkExists(BookmarkNode* root, const char* url);
void addBookmark(BookmarkNode** root, TabList* tabs);
void viewBookmarks(BookmarkNode* root, HistoryList* history, TabList* tabs, HashTable* cache, BackStack* stack);
void freeBookmarks(BookmarkNode* root);
void saveData(HistoryList* history, BookmarkNode* root);
void loadData(HistoryList* history, BookmarkNode** root);

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void showHeader() {
    clearScreen();
    printf("%s", HEADER_COLOR[theme]);
    printf("=============================================\n");
    printf("|      Pranav's Surf Browser v4.1           |\n");
    printf("|  Powered by Advanced C Data Structures    |\n");
    printf("=============================================\n");
    printf("%s", ANSI_COLOR_RESET);
    printf("\n");
}

void showFakeFeatures() {
    printf("%s", TEXT_COLOR[theme]);
    printf("--- Advanced Features ---\n");
    printf("  * SSL Encryption: Secure Browsing Enabled\n");
    printf("  * CloudSync Serverless Backend: Active\n");
    printf("  * Firewall Protection: Intrusion Blocked\n");
    printf("  * AI-Powered Threat Detection: Running\n");
    printf("-------------------------\n\n");
    printf("%s", ANSI_COLOR_RESET);
}

// History Management
void initHistory(HistoryList* history) {
    history->head = NULL;
    history->current = NULL;
    history->size = 0;
}

void addToHistory(HistoryList* history, const char* url) {
    HistoryNode* newNode = (HistoryNode*)malloc(sizeof(HistoryNode));
    if (!newNode) {
        printf(ANSI_COLOR_RED "Memory allocation failed!\n" ANSI_COLOR_RESET);
        return;
    }
    strncpy(newNode->url, url, URL_LEN - 1);
    newNode->url[URL_LEN - 1] = '\0';
    newNode->next = NULL;

    if (history->head == NULL) {
        history->head = newNode;
        history->current = newNode;
    } else {
        history->current->next = newNode;
        history->current = newNode;
    }
    history->size++;
    saveData(history, NULL); // Save history after adding
}

void showHistory(HistoryList* history) {
    printf("%s", TEXT_COLOR[theme]);
    printf("Browsing History:\n");
    printf("----------------\n");
    if (history->size == 0) {
        printf("No history yet!\n");
    } else {
        HistoryNode* temp = history->head;
        int i = 1;
        while (temp) {
            printf("%s%d. %s\n", (temp == history->current) ? "> " : "  ", i++, temp->url);
            temp = temp->next;
        }
    }
    printf("----------------\n\n");
    printf("%s", ANSI_COLOR_RESET);
}

void clearHistory(HistoryList* history) {
    while (history->head) {
        HistoryNode* temp = history->head;
        history->head = history->head->next;
        free(temp);
    }
    history->head = NULL;
    history->current = NULL;
    history->size = 0;
    saveData(history, NULL); // Update saved data
    printf(ANSI_COLOR_GREEN "History cleared successfully!\n" ANSI_COLOR_RESET);
}

void searchHistory(HistoryList* history, TabList* tabs, HashTable* cache, BackStack* stack) {
    char keyword[URL_LEN];
    printf("%sEnter search keyword: %s", TEXT_COLOR[theme], ANSI_COLOR_RESET);
    scanf("%s", keyword);
    
    clearScreen();
    showHeader();
    printf("%sHistory matching '%s':\n", TEXT_COLOR[theme], keyword);
    printf("----------------\n");
    HistoryNode* temp = history->head;
    int found = 0;
    int index = 1;
    while (temp) {
        if (strstr(temp->url, keyword)) {
            printf("  %d. %s\n", index++, temp->url);
            found++;
        }
        temp = temp->next;
    }
    if (!found) {
        printf("No matches found.\n");
    } else {
        printf("----------------\n");
        printf("Enter number to visit URL (0 to cancel): ");
        int choice;
        scanf("%d", &choice);
        if (choice > 0 && choice < index) {
            temp = history->head;
            int i = 1;
            while (temp && i < choice) {
                temp = temp->next;
                i++;
            }
            if (temp) {
                addToHistory(history, temp->url);
                addTab(tabs, temp->url);
                pushBackStack(stack, temp->url);
                fetchContent(cache, temp->url);
            }
        }
    }
    printf("----------------\n%s", ANSI_COLOR_RESET);
    printf("Press Enter to continue...");
    getchar();
    getchar();
}

// Tab Management
void initTabs(TabList* tabs) {
    tabs->current = NULL;
    tabs->tab_count = 0;
}

void addTab(TabList* tabs, const char* url) {
    if (tabs->tab_count >= MAX_TABS) {
        printf(ANSI_COLOR_RED "Maximum tabs reached!\n" ANSI_COLOR_RESET);
        return;
    }

    TabNode* newTab = (TabNode*)malloc(sizeof(TabNode));
    if (!newTab) {
        printf(ANSI_COLOR_RED "Memory allocation failed!\n" ANSI_COLOR_RESET);
        return;
    }
    strncpy(newTab->url, url, URL_LEN - 1);
    newTab->url[URL_LEN - 1] = '\0';

    if (tabs->current == NULL) {
        newTab->next = newTab;
        newTab->prev = newTab;
        tabs->current = newTab;
    } else {
        newTab->next = tabs->current->next;
        newTab->prev = tabs->current;
        tabs->current->next->prev = newTab;
        tabs->current->next = newTab;
        tabs->current = newTab;
    }
    tabs->tab_count++;
}

void switchTab(TabList* tabs, int index) {
    if (index < 1 || index > tabs->tab_count) {
        printf(ANSI_COLOR_RED "Invalid tab index!\n" ANSI_COLOR_RESET);
        return;
    }
    TabNode* temp = tabs->current;
    for (int i = 1; i < index; i++) {
        temp = temp->next;
    }
    tabs->current = temp;
}

void showTabs(TabList* tabs) {
    printf("%s", TEXT_COLOR[theme]);
    printf("Open Tabs:\n");
    printf("----------------\n");
    if (tabs->tab_count == 0) {
        printf("No tabs open!\n");
    } else {
        TabNode* temp = tabs->current;
        int i = 1;
        do {
            printf("%s%d. %s\n", (temp == tabs->current) ? "> " : "  ", i++, temp->url);
            temp = temp->next;
        } while (temp != tabs->current);
    }
    printf("----------------\n\n");
    printf("%s", ANSI_COLOR_RESET);
}

// Hash Table for URL Caching
void initHashTable(HashTable* cache) {
    for (int i = 0; i < HASH_SIZE; i++) {
        cache->table[i] = NULL;
    }
}

unsigned int hash(const char* url) {
    unsigned int hash = 0;
    while (*url) {
        hash = (hash * 31 + *url) % HASH_SIZE;
        url++;
    }
    return hash;
}

void addToCache(HashTable* cache, const char* url, const char* content) {
    unsigned int index = hash(url);
    CacheNode* newNode = (CacheNode*)malloc(sizeof(CacheNode));
    if (!newNode) {
        printf(ANSI_COLOR_RED "Memory allocation failed!\n" ANSI_COLOR_RESET);
        return;
    }
    strncpy(newNode->url, url, URL_LEN - 1);
    newNode->url[URL_LEN - 1] = '\0';
    strncpy(newNode->content, content, CONTENT_LEN - 1);
    newNode->content[CONTENT_LEN - 1] = '\0';
    newNode->next = cache->table[index];
    cache->table[index] = newNode;
}

char* getFromCache(HashTable* cache, const char* url) {
    unsigned int index = hash(url);
    CacheNode* temp = cache->table[index];
    while (temp) {
        if (strcmp(temp->url, url) == 0) {
            return temp->content;
        }
        temp = temp->next;
    }
    return NULL;
}

// Backtracking Stack
void initBackStack(BackStack* stack) {
    stack->top = NULL;
}

void pushBackStack(BackStack* stack, const char* url) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    if (!newNode) {
        printf(ANSI_COLOR_RED "Memory allocation failed!\n" ANSI_COLOR_RESET);
        return;
    }
    strncpy(newNode->url, url, URL_LEN - 1);
    newNode->url[URL_LEN - 1] = '\0';
    newNode->next = stack->top;
    stack->top = newNode;
}

char* popBackStack(BackStack* stack) {
    if (!stack->top) return NULL;
    StackNode* temp = stack->top;
    char* url = (char*)malloc(URL_LEN * sizeof(char));
    if (!url) {
        printf(ANSI_COLOR_RED "Memory allocation failed!\n" ANSI_COLOR_RESET);
        return NULL;
    }
    strncpy(url, temp->url, URL_LEN - 1);
    url[URL_LEN - 1] = '\0';
    stack->top = temp->next;
    free(temp);
    return url;
}

// Bookmark Management
BookmarkNode* initBookmarks() {
    return NULL;
}

BookmarkNode* insertBookmark(BookmarkNode* root, const char* url) {
    if (!root) {
        BookmarkNode* newNode = (BookmarkNode*)malloc(sizeof(BookmarkNode));
        if (!newNode) {
            printf(ANSI_COLOR_RED "Memory allocation failed!\n" ANSI_COLOR_RESET);
            return NULL;
        }
        strncpy(newNode->url, url, URL_LEN - 1);
        newNode->url[URL_LEN - 1] = '\0';
        newNode->left = newNode->right = NULL;
        return newNode;
    }
    if (strcmp(url, root->url) < 0)
        root->left = insertBookmark(root->left, url);
    else if (strcmp(url, root->url) > 0)
        root->right = insertBookmark(root->right, url);
    return root;
}

int bookmarkExists(BookmarkNode* root, const char* url) {
    if (!root) return 0;
    if (strcmp(url, root->url) == 0) return 1;
    return bookmarkExists(root->left, url) || bookmarkExists(root->right, url);
}

void addBookmark(BookmarkNode** root, TabList* tabs) {
    if (!tabs->current) {
        printf(ANSI_COLOR_RED "No active tab to bookmark!\n" ANSI_COLOR_RESET);
        return;
    }
    if (bookmarkExists(*root, tabs->current->url)) {
        printf(ANSI_COLOR_RED "URL already bookmarked!\n" ANSI_COLOR_RESET);
    } else {
        *root = insertBookmark(*root, tabs->current->url);
        saveData(NULL, *root); // Save bookmarks
        printf(ANSI_COLOR_GREEN "Bookmark added: %s\n" ANSI_COLOR_RESET, tabs->current->url);
    }
    printf("Press Enter to continue...");
    getchar();
    getchar();
}

void viewBookmarks(BookmarkNode* root, HistoryList* history, TabList* tabs, HashTable* cache, BackStack* stack) {
    clearScreen();
    showHeader();
    printf("%sBookmarked URLs:\n", TEXT_COLOR[theme]);
    printf("----------------\n");
    
    if (!root) {
        printf("No bookmarks yet!\n");
    } else {
        BookmarkNode* bookmarks[100];
        int count = 0;
        void collectBookmarks(BookmarkNode* node) {
            if (node) {
                collectBookmarks(node->left);
                bookmarks[count++] = node;
                collectBookmarks(node->right);
            }
        }
        collectBookmarks(root);
        
        for (int i = 0; i < count; i++) {
            printf("  %d. %s\n", i + 1, bookmarks[i]->url);
        }
        printf("----------------\n");
        printf("Enter number to visit bookmark (0 to cancel): ");
        int choice;
        scanf("%d", &choice);
        if (choice > 0 && choice <= count) {
            addToHistory(history, bookmarks[choice - 1]->url);
            addTab(tabs, bookmarks[choice - 1]->url);
            pushBackStack(stack, bookmarks[choice - 1]->url);
            fetchContent(cache, bookmarks[choice - 1]->url);
        }
    }
    printf("----------------\n%s", ANSI_COLOR_RESET);
    printf("Press Enter to continue...");
    getchar();
    getchar();
}

void freeBookmarks(BookmarkNode* root) {
    if (root) {
        freeBookmarks(root->left);
        freeBookmarks(root->right);
        free(root);
    }
}

// File-Based Database
void saveData(HistoryList* history, BookmarkNode* root) {
    FILE* fp = fopen("browser_data.txt", "w");
    if (!fp) {
        printf(ANSI_COLOR_RED "Failed to save data!\n" ANSI_COLOR_RESET);
        return;
    }
    HistoryNode* h = history ? history->head : NULL;
    while (h) {
        fprintf(fp, "H:%s\n", h->url);
        h = h->next;
    }
    void saveBookmarks(BookmarkNode* node) {
        if (node) {
            fprintf(fp, "B:%s\n", node->url);
            saveBookmarks(node->left);
            saveBookmarks(node->right);
        }
    }
    if (root) saveBookmarks(root);
    fclose(fp);
}

void loadData(HistoryList* history, BookmarkNode** root) {
    FILE* fp = fopen("browser_data.txt", "r");
    if (!fp) return;
    char line[URL_LEN + 2];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = 0;
        if (line[0] == 'H') {
            addToHistory(history, line + 2);
        } else if (line[0] == 'B') {
            *root = insertBookmark(*root, line + 2);
        }
    }
    fclose(fp);
}

// Open URL in external browser
void openURLInBrowser(const char* url) {
    char command[URL_LEN + 50];
    char formatted_url[URL_LEN];
    
    if (strncmp(url, "http://", 7) != 0 && strncmp(url, "https://", 8) != 0) {
        snprintf(formatted_url, URL_LEN, "https://%s", url);
    } else {
        strncpy(formatted_url, url, URL_LEN - 1);
        formatted_url[URL_LEN - 1] = '\0';
    }

    #ifdef _WIN32
        if (browser_choice == 1) {
            snprintf(command, sizeof(command), "start msedge \"%s\"", formatted_url);
        } else if (browser_choice == 2) {
            snprintf(command, sizeof(command), "start chrome \"%s\"", formatted_url);
        } else {
            snprintf(command, sizeof(command), "start \"\" \"%s\"", formatted_url);
        }
    #elif __APPLE__
        if (browser_choice == 1) {
            snprintf(command, sizeof(command), "open -a \"Microsoft Edge\" \"%s\"", formatted_url);
        } else if (browser_choice == 2) {
            snprintf(command, sizeof(command), "open -a \"Google Chrome\" \"%s\"", formatted_url);
        } else {
            snprintf(command, sizeof(command), "open \"%s\"", formatted_url);
        }
    #else
        if (browser_choice == 1) {
            snprintf(command, sizeof(command), "microsoft-edge \"%s\"", formatted_url);
        } else if (browser_choice == 2) {
            snprintf(command, sizeof(command), "google-chrome \"%s\"", formatted_url);
        } else {
            snprintf(command, sizeof(command), "xdg-open \"%s\"", formatted_url);
        }
    #endif

    system(command);
}

void fetchContent(HashTable* cache, const char* url) {
    clearScreen();
    showHeader();
    
    printf(ANSI_COLOR_GREEN);
    printf("Current URL: %s\n", url);
    printf(ANSI_COLOR_RESET);
    
    printf("%sCurrent Time: Fri May 23 23:10:00 IST 2025\n\n", TEXT_COLOR[theme]);
    
    if (strcmp(url, "home") != 0) {
        openURLInBrowser(url);
    }
    
    printf("Web Content:\n");
    printf("=============================================\n");
    
    char* cachedContent = getFromCache(cache, url);
    if (cachedContent) {
        printf("%s", cachedContent);
    } else {
        char content[CONTENT_LEN] = "";
        if (strcmp(url, "home") == 0) {
            snprintf(content, CONTENT_LEN, 
                "| Welcome to Pranav's Surf Browser!          |\n"
                "| Start your secure browsing journey.        |\n"
                "| Powered by:                                |\n"
                "| - Serverless CloudSync Architecture        |\n"
                "| - Advanced Cybersecurity Suite             |\n"
                "| - AI-Driven Threat Detection              |\n");
            showFakeFeatures();
        } else if (strcmp(url, "google.com") == 0) {
            snprintf(content, CONTENT_LEN, 
                "| Welcome to Google Search!                   |\n"
                "| Search the world's information instantly.   |\n");
        } else if (strcmp(url, "openai.com") == 0) {
            snprintf(content, CONTENT_LEN, 
                "| OpenAI - Pioneering AI Research            |\n"
                "| Explore ChatGPT, Codex & more AI tools.    |\n");
        } else {
            snprintf(content, CONTENT_LEN, 
                ANSI_COLOR_RED
                "| 404 Not Found!                            |\n"
                "| This is a simulated browser environment.   |\n"
                ANSI_COLOR_RESET);
        }
        printf("%s", content);
        addToCache(cache, url, content);
    }
    
    printf("=============================================\n\n");
}

void showMenu() {
    printf("%s", MENU_COLOR[theme]);
    printf("Navigation Menu:\n");
    printf("----------------------------\n");
    printf("| [1] Enter New URL        |\n");
    printf("| [2] Refresh Page         |\n");
    printf("| [3] Go Back              |\n");
    printf("| [4] Go Home              |\n");
    printf("| [5] New Tab              |\n");
    printf("| [6] Switch Tab           |\n");
    printf("| [7] Set Browser          |\n");
    printf("| [8] Clear History        |\n");
    printf("| [9] Search History       |\n");
    printf("| [10] Change Theme        |\n");
    printf("| [11] Add Bookmark        |\n");
    printf("| [12] View Bookmarks      |\n");
    printf("| [13] Exit Browser        |\n");
    printf("----------------------------\n");
    printf("%sEnter choice (1-13): %s", TEXT_COLOR[theme], ANSI_COLOR_RESET);
}

int validateURL(const char* url) {
    if (strlen(url) < 3 || strlen(url) >= URL_LEN) return 0;
    for (int i = 0; url[i]; i++) {
        if (!isalnum(url[i]) && url[i] != '.' && url[i] != '-' && url[i] != '/' && url[i] != ':' && url[i] != '?' && url[i] != '&' && url[i] != '=') return 0;
    }
    return 1;
}

void suggestURLs(HistoryList* history, const char* prefix) {
    printf("%sSuggested URLs:\n", TEXT_COLOR[theme]);
    printf("----------------\n");
    HistoryNode* temp = history->head;
    int count = 0;
    while (temp && count < MAX_SUGGESTIONS) {
        if (strncmp(temp->url, prefix, strlen(prefix)) == 0) {
            printf("  %s\n", temp->url);
            count++;
        }
        temp = temp->next;
    }
    if (count == 0) printf("  No suggestions found.\n");
    printf("----------------\n\n%s", ANSI_COLOR_RESET);
}

void enterURL(HistoryList* history, TabList* tabs, HashTable* cache, BackStack* stack) {
    char temp_url[URL_LEN];
    printf("%sEnter URL (e.g., google.com, https://example.com): %s", TEXT_COLOR[theme], ANSI_COLOR_RESET);
    
    scanf("%s", temp_url);
    
    suggestURLs(history, temp_url);
    
    if (validateURL(temp_url)) {
        addToHistory(history, temp_url);
        addTab(tabs, temp_url);
        pushBackStack(stack, temp_url);
        fetchContent(cache, temp_url);
        showTabs(tabs);
        showHistory(history);
    } else {
        printf(ANSI_COLOR_RED "\nInvalid URL format! Please try again.\n" ANSI_COLOR_RESET);
    }
}

void goBack(HistoryList* history, TabList* tabs, BackStack* stack) {
    char* prev_url = popBackStack(stack);
    if (prev_url) {
        HistoryNode* temp = history->head;
        while (temp && temp->next != history->current) {
            temp = temp->next;
        }
        if (temp) {
            history->current = temp;
            addTab(tabs, prev_url);
            fetchContent(NULL, prev_url);
            showTabs(tabs);
            showHistory(history);
        }
        free(prev_url);
    } else {
        printf(ANSI_COLOR_RED "\nNo more history to go back!\n" ANSI_COLOR_RESET);
    }
}

void goHome(HistoryList* history, TabList* tabs, HashTable* cache, BackStack* stack) {
    addToHistory(history, "home");
    addTab(tabs, "home");
    pushBackStack(stack, "home");
    fetchContent(cache, "home");
    showTabs(tabs);
    showHistory(history);
}

void setBrowser() {
    clearScreen();
    showHeader();
    printf("%sSelect Browser:\n", TEXT_COLOR[theme]);
    printf("----------------\n");
    printf("  0. Default Browser\n");
    printf("  1. Microsoft Edge\n");
    printf("  2. Google Chrome\n");
    printf("----------------\n");
    printf("Enter choice (0-2): %s", ANSI_COLOR_RESET);
    
    int choice;
    if (scanf("%d", &choice) != 1 || choice < 0 || choice > 2) {
        printf(ANSI_COLOR_RED "Invalid choice! Keeping current browser.\n" ANSI_COLOR_RESET);
    } else {
        browser_choice = choice;
        printf(ANSI_COLOR_GREEN "Browser set to %s!\n" ANSI_COLOR_RESET, 
               choice == 0 ? "Default" : (choice == 1 ? "Microsoft Edge" : "Google Chrome"));
    }
    getchar();
    printf("Press Enter to continue...");
    getchar();
}

void changeTheme() {
    clearScreen();
    showHeader();
    printf("%sSelect Theme:\n", TEXT_COLOR[theme]);
    printf("----------------\n");
    printf("  0. Default (Cyan/Yellow)\n");
    printf("  1. Dark (Blue/Cyan)\n");
    printf("  2. Light (Green/Magenta)\n");
    printf("----------------\n");
    printf("Enter choice (0-2): %s", ANSI_COLOR_RESET);
    
    int choice;
    if (scanf("%d", &choice) != 1 || choice < 0 || choice > 2) {
        printf(ANSI_COLOR_RED "Invalid choice! Keeping current theme.\n" ANSI_COLOR_RESET);
    } else {
        theme = choice;
        printf(ANSI_COLOR_GREEN "Theme set to %s!\n" ANSI_COLOR_RESET, 
               choice == 0 ? "Default" : (choice == 1 ? "Dark" : "Light"));
    }
    getchar();
    printf("Press Enter to continue...");
    getchar();
}

int main() {
    setlocale(LC_ALL, "");

    HistoryList history;
    TabList tabs;
    HashTable cache;
    BackStack stack;
    BookmarkNode* bookmark_root = initBookmarks();
    
    initHistory(&history);
    initTabs(&tabs);
    initHashTable(&cache);
    initBackStack(&stack);
    
    loadData(&history, &bookmark_root);
    goHome(&history, &tabs, &cache, &stack);
    
    while (1) {
        showMenu();
        int choice;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf(ANSI_COLOR_RED "Invalid input! Please enter a number.\n" ANSI_COLOR_RESET);
            continue;
        }
        
        switch (choice) {
            case 1:
                while (getchar() != '\n');
                enterURL(&history, &tabs, &cache, &stack);
                break;
            case 2:
                fetchContent(&cache, tabs.current->url);
                showTabs(&tabs);
                showHistory(&history);
                break;
            case 3:
                goBack(&history, &tabs, &stack);
                break;
            case 4:
                goHome(&history, &tabs, &cache, &stack);
                break;
            case 5:
                while (getchar() != '\n');
                enterURL(&history, &tabs, &cache, &stack);
                break;
            case 6:
                showTabs(&tabs);
                printf("Enter tab number to switch to: ");
                int tab_index;
                scanf("%d", &tab_index);
                switchTab(&tabs, tab_index);
                fetchContent(&cache, tabs.current->url);
                showTabs(&tabs);
                showHistory(&history);
                break;
            case 7:
                setBrowser();
                break;
            case 8:
                clearHistory(&history);
                printf("Press Enter to continue...");
                getchar();
                getchar();
                clearScreen();
                showHeader();
                showTabs(&tabs);
                showHistory(&history);
                break;
            case 9:
                searchHistory(&history, &tabs, &cache, &stack);
                clearScreen();
                showHeader();
                showTabs(&tabs);
                showHistory(&history);
                break;
            case 10:
                changeTheme();
                clearScreen();
                showHeader();
                showTabs(&tabs);
                showHistory(&history);
                break;
            case 11:
                addBookmark(&bookmark_root, &tabs);
                clearScreen();
                showHeader();
                showTabs(&tabs);
                showHistory(&history);
                break;
            case 12:
                viewBookmarks(bookmark_root, &history, &tabs, &cache, &stack);
                clearScreen();
                showHeader();
                showTabs(&tabs);
                showHistory(&history);
                break;
            case 13:
                clearScreen();
                printf(ANSI_COLOR_GREEN "\nThanks for browsing, Pranav! Goodbye!\n" ANSI_COLOR_RESET);
                while (history.head) {
                    HistoryNode* temp = history.head;
                    history.head = history.head->next;
                    free(temp);
                }
                while (tabs.tab_count > 0) {
                    TabNode* temp = tabs.current;
                    tabs.current->prev->next = tabs.current->next;
                    tabs.current->next->prev = tabs.current->prev;
                    tabs.current = tabs.current->next;
                    free(temp);
                    tabs.tab_count--;
                }
                for (int i = 0; i < HASH_SIZE; i++) {
                    while (cache.table[i]) {
                        CacheNode* temp = cache.table[i];
                        cache.table[i] = cache.table[i]->next;
                        free(temp);
                    }
                }
                while (stack.top) {
                    StackNode* temp = stack.top;
                    stack.top = stack.top->next;
                    free(temp);
                }
                freeBookmarks(bookmark_root);
                exit(0);
            default:
                printf(ANSI_COLOR_RED "Invalid option! Choose 1-13.\n" ANSI_COLOR_RESET);
        }
    }
    
    return 0;
}