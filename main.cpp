#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <limits>

using namespace std;

// ANSI escape codes for beautiful terminal output
const string RESET = "\033[0m";
const string BOLD = "\033[1m";
const string CYAN = "\033[36m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string MAGENTA = "\033[35m";
const string RED = "\033[31m";
const string BLUE = "\033[34m";

// Structure to hold User Data
struct User {
    string name;
    unordered_set<string> interests;
};

// Structure to hold Recommendation Data
struct Recommendation {
    string id;
    string name;
    int mutualFriends;
    int sharedInterests;
    int score;

    // Sort descending by score
    bool operator<(const Recommendation& other) const {
        return score > other.score;
    }
};

class SocialGraph {
private:
    // Adjacency list: UserID -> Set of Friend UserIDs
    unordered_map<string, unordered_set<string>> adjacencyList;
    // User Map: UserID -> User Object
    unordered_map<string, User> users;

    // Helper function to calculate intersection of two sets
    int getIntersectionSize(const unordered_set<string>& set1, const unordered_set<string>& set2) const {
        int count = 0;
        const auto& smaller = (set1.size() < set2.size()) ? set1 : set2;
        const auto& larger = (set1.size() < set2.size()) ? set2 : set1;

        for (const string& item : smaller) {
            if (larger.find(item) != larger.end()) {
                count++;
            }
        }
        return count;
    }

public:
    void addUser(const string& userId, const string& name, const vector<string>& interests) {
        if (users.find(userId) == users.end()) {
            User newUser;
            newUser.name = name;
            for (const string& interest : interests) {
                newUser.interests.insert(interest);
            }
            users[userId] = newUser;
            adjacencyList[userId] = unordered_set<string>();
        }
    }

    void addFriendship(const string& user1, const string& user2) {
        if (users.find(user1) != users.end() && users.find(user2) != users.end()) {
            adjacencyList[user1].insert(user2);
            adjacencyList[user2].insert(user1);
        } else {
            cout << RED << "Error: One or both users do not exist!" << RESET << endl;
        }
    }

    int getMutualFriends(const string& user1, const string& user2) const {
        if (adjacencyList.find(user1) == adjacencyList.end() || adjacencyList.find(user2) == adjacencyList.end()) return 0;
        return getIntersectionSize(adjacencyList.at(user1), adjacencyList.at(user2));
    }

    int getSharedInterests(const string& user1, const string& user2) const {
        if (users.find(user1) == users.end() || users.find(user2) == users.end()) return 0;
        return getIntersectionSize(users.at(user1).interests, users.at(user2).interests);
    }

    // -----------------------------------------------------------------
    // FEATURE 1: Recommendation Engine
    // -----------------------------------------------------------------
    vector<Recommendation> getRecommendations(const string& targetUserId) const {
        vector<Recommendation> recommendations;
        if (users.find(targetUserId) == users.end()) return recommendations;

        const auto& targetFriends = adjacencyList.at(targetUserId);
        const int MUTUAL_FRIEND_WEIGHT = 2;
        const int SHARED_INTEREST_WEIGHT = 1;

        for (const auto& pair : users) {
            const string& otherUserId = pair.first;
            const User& otherUser = pair.second;

            if (otherUserId == targetUserId || targetFriends.find(otherUserId) != targetFriends.end()) {
                continue;
            }

            int mutualFriendsCount = getMutualFriends(targetUserId, otherUserId);
            int sharedInterestsCount = getSharedInterests(targetUserId, otherUserId);

            if (mutualFriendsCount > 0 || sharedInterestsCount > 0) {
                int score = (mutualFriendsCount * MUTUAL_FRIEND_WEIGHT) + 
                            (sharedInterestsCount * SHARED_INTEREST_WEIGHT);

                recommendations.push_back({otherUserId, otherUser.name, mutualFriendsCount, sharedInterestsCount, score});
            }
        }
        sort(recommendations.begin(), recommendations.end());
        return recommendations;
    }

    // -----------------------------------------------------------------
    // FEATURE 2: Degrees of Separation (Shortest Path via BFS)
    // -----------------------------------------------------------------
    // Time Complexity: O(V + E) 
    int getDegreesOfSeparation(const string& startUserId, const string& targetUserId) const {
        if (users.find(startUserId) == users.end() || users.find(targetUserId) == users.end()) return -1;
        if (startUserId == targetUserId) return 0;

        unordered_set<string> visited;
        queue<pair<string, int>> q; // Queue of {userId, distance}

        q.push({startUserId, 0});
        visited.insert(startUserId);

        while (!q.empty()) {
            auto [currentId, distance] = q.front();
            q.pop();

            if (currentId == targetUserId) {
                return distance;
            }

            for (const string& neighbor : adjacencyList.at(currentId)) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    q.push({neighbor, distance + 1});
                }
            }
        }
        return -1; // Unreachable
    }

    // -----------------------------------------------------------------
    // FEATURE 3: Community Detection (Connected Components via BFS)
    // -----------------------------------------------------------------
    // Time Complexity: O(V + E)
    vector<vector<string>> getCommunities() const {
        unordered_set<string> visited;
        vector<vector<string>> communities;

        for (const auto& pair : users) {
            const string& startUserId = pair.first;
            
            if (visited.find(startUserId) == visited.end()) {
                vector<string> currentCommunity;
                queue<string> q;
                
                q.push(startUserId);
                visited.insert(startUserId);

                while (!q.empty()) {
                    string currentId = q.front();
                    q.pop();
                    currentCommunity.push_back(currentId);

                    for (const string& neighbor : adjacencyList.at(currentId)) {
                        if (visited.find(neighbor) == visited.end()) {
                            visited.insert(neighbor);
                            q.push(neighbor);
                        }
                    }
                }
                communities.push_back(currentCommunity);
            }
        }
        return communities;
    }

    // --- Helper UI Methods ---
    void displayUsers() const {
        cout << BOLD << "\n--- Available Users ---" << RESET << endl;
        for (const auto& pair : users) {
            cout << YELLOW << pair.first << RESET << ": " << pair.second.name << endl;
        }
    }

    bool userExists(const string& id) const {
        return users.find(id) != users.end();
    }
    
    string getUserName(const string& id) const {
        return users.at(id).name;
    }
};

void seedData(SocialGraph& graph) {
    // Community 1
    graph.addUser("u1", "Alice", {"Coding", "Music", "Reading"});
    graph.addUser("u2", "Bob", {"Music", "Sports"});
    graph.addUser("u3", "Charlie", {"Coding", "Gaming", "Music"});
    graph.addUser("u4", "David", {"Reading", "Travel"});
    graph.addUser("u5", "Eve", {"Sports", "Gaming", "Coding"});
    graph.addUser("u6", "Frank", {"Travel", "Music", "Reading"});

    graph.addFriendship("u1", "u2"); // Alice - Bob
    graph.addFriendship("u2", "u3"); // Bob - Charlie
    graph.addFriendship("u1", "u4"); // Alice - David
    graph.addFriendship("u3", "u5"); // Charlie - Eve
    graph.addFriendship("u4", "u6"); // David - Frank

    // Community 2 (Disconnected from Community 1)
    graph.addUser("u7", "Grace", {"Art", "Design"});
    graph.addUser("u8", "Heidi", {"Art", "Photography"});
    graph.addFriendship("u7", "u8");
}

int main() {
    SocialGraph graph;
    seedData(graph);

    cout << BOLD << MAGENTA << "===========================================" << RESET << endl;
    cout << BOLD << MAGENTA << "  DSA Friend Suggestion & Network Analysis " << RESET << endl;
    cout << BOLD << MAGENTA << "===========================================" << RESET << endl;

    while (true) {
        cout << BOLD << "\nMenu Options:" << RESET << endl;
        cout << CYAN << "1." << RESET << " Get Friend Suggestions" << endl;
        cout << CYAN << "2." << RESET << " Find Degrees of Separation (Shortest Path)" << endl;
        cout << CYAN << "3." << RESET << " View All Communities (Connected Components)" << endl;
        cout << CYAN << "4." << RESET << " Add New User" << endl;
        cout << CYAN << "5." << RESET << " Add New Friendship" << endl;
        cout << CYAN << "6." << RESET << " Exit" << endl;
        cout << BOLD << "Enter your choice: " << RESET;

        int choice;
        if (!(cin >> choice)) {
            cin.clear(); // clear error flags
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ignore invalid input
            cout << RED << "Invalid input. Please enter a number." << RESET << endl;
            continue;
        }

        if (choice == 1) {
            graph.displayUsers();
            string userId;
            cout << "Enter User ID: ";
            cin >> userId;

            if (!graph.userExists(userId)) {
                cout << RED << "User not found!" << RESET << endl;
                continue;
            }

            auto recs = graph.getRecommendations(userId);
            cout << BOLD << GREEN << "\nTop Friend Suggestions for " << graph.getUserName(userId) << ":" << RESET << endl;
            if (recs.empty()) cout << "No recommendations found." << endl;
            for (size_t i = 0; i < recs.size(); i++) {
                cout << i + 1 << ". " << recs[i].name << " (Score: " << recs[i].score << ")" 
                     << " [Mutuals: " << recs[i].mutualFriends << ", Shared Interests: " << recs[i].sharedInterests << "]" << endl;
            }
        } 
        else if (choice == 2) {
            graph.displayUsers();
            string u1, u2;
            cout << "Enter first User ID: "; cin >> u1;
            cout << "Enter second User ID: "; cin >> u2;

            if (!graph.userExists(u1) || !graph.userExists(u2)) {
                cout << RED << "One or both users not found!" << RESET << endl;
                continue;
            }

            int dist = graph.getDegreesOfSeparation(u1, u2);
            cout << BOLD << BLUE << "\nDegrees of separation between " << graph.getUserName(u1) << " and " << graph.getUserName(u2) << ":" << RESET << endl;
            if (dist == -1) {
                cout << "No connection path exists between them." << endl;
            } else {
                cout << dist << " degree(s) of separation." << endl;
            }
        } 
        else if (choice == 3) {
            auto communities = graph.getCommunities();
            cout << BOLD << YELLOW << "\nFound " << communities.size() << " Isolated Communities:" << RESET << endl;
            for (size_t i = 0; i < communities.size(); i++) {
                cout << "Community " << i + 1 << ": ";
                for (size_t j = 0; j < communities[i].size(); j++) {
                    cout << graph.getUserName(communities[i][j]);
                    if (j < communities[i].size() - 1) cout << ", ";
                }
                cout << endl;
            }
        } 
        else if (choice == 4) {
            string id, name, interestInput;
            vector<string> interests;
            cout << "Enter new User ID (e.g. u10): "; cin >> id;
            cout << "Enter User Name (no spaces): "; cin >> name;
            cout << "Enter Interests (comma separated, no spaces): "; cin >> interestInput;
            
            size_t pos = 0;
            while ((pos = interestInput.find(",")) != string::npos) {
                interests.push_back(interestInput.substr(0, pos));
                interestInput.erase(0, pos + 1);
            }
            if (!interestInput.empty()) interests.push_back(interestInput);

            graph.addUser(id, name, interests);
            cout << BOLD << GREEN << "Successfully added user " << name << "!" << RESET << endl;
        }
        else if (choice == 5) {
            graph.displayUsers();
            string u1, u2;
            cout << "Enter first User ID: "; cin >> u1;
            cout << "Enter second User ID: "; cin >> u2;
            
            if (!graph.userExists(u1) || !graph.userExists(u2)) {
                cout << RED << "One or both users not found!" << RESET << endl;
                continue;
            }
            
            graph.addFriendship(u1, u2);
            cout << BOLD << GREEN << "Successfully added friendship between " << graph.getUserName(u1) << " and " << graph.getUserName(u2) << "!" << RESET << endl;
        }
        else if (choice == 6) {
            cout << GREEN << "Exiting. Good luck on your interview!" << RESET << endl;
            break;
        } 
        else {
            cout << RED << "Invalid choice." << RESET << endl;
        }
    }

    return 0;
}
