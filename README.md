# DSA Friend Suggestion & Network Analysis

A C++ terminal application demonstrating advanced Data Structures and Algorithms through a social network graph. This project was built to showcase graph theory, optimized lookups, and real-time algorithmic calculations.

## 🚀 Features

- **Persistent SQL Storage (SQLite3)**: Automatically connects to a local `.db` file, creates tables, and persists users and friendships across sessions.
- **Friend Recommendations**: Uses a custom weighted algorithm `(Mutual Friends * 2) + (Shared Interests * 1)` to score and recommend potential friends.
- **Degrees of Separation**: Implements Breadth-First Search (BFS) to find the shortest connection path between any two users in the network.
- **Community Detection**: Uses Graph Traversal (BFS) to identify isolated, disconnected groups of friends (Connected Components) within the network.
- **Interactive UI**: A fully functional, colored terminal menu allowing users to dynamically add new users, create friendships, and query the graph in real-time.

## 🧠 Data Structures & Algorithms Used

- **Adjacency List (`std::unordered_map` & `std::unordered_set`)**: Used to represent the social graph, allowing O(1) time complexity for edge lookups and neighbor retrieval.
- **Set Intersections**: Optimized O(min(N, M)) intersection algorithm to calculate mutual friends and shared interests.
- **Breadth-First Search (BFS)**: Applied using `std::queue` for shortest path calculations and community subgraph detection.
- **Sorting**: Uses `std::sort` to rank recommendations in descending order based on calculated scores.

## 🛠️ How to Run

1. Clone the repository.
2. Compile the C++ code using a modern compiler (C++17 or higher) and link the SQLite library:
   ```bash
   g++ -std=c++17 main.cpp -lsqlite3 -o friend_suggestion
   ```
3. Run the interactive executable:
   ```bash
   ./friend_suggestion
   ```

## 📋 Menu Options

Once running, you can interact with the graph using the following options:
1. **Get Friend Suggestions**: See ranked recommendations for any user.
2. **Find Degrees of Separation**: Find the shortest path between two users.
3. **View All Communities**: See all disconnected sub-graphs in the network.
4. **Add New User**: Dynamically insert a new node into the graph.
5. **Add New Friendship**: Dynamically insert a new edge between two nodes.
6. **Exit**: Close the program.
