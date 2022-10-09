#ifndef _HUFFMAN_TREE_H_
#define _HUFFMAN_TREE_H_
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <queue>
#include <vector>


struct Node {
	Node(char character_ = -64, int frequency_ = 0); //Default parameters that set character to -64 and frequency to zero, any char value between -1 and -128 would work as the character
	char character;
	int frequency;
	Node* right;
	Node* left;
};

struct Compare {
	bool operator()(const Node* left, const Node* right) const; //Comparison class so that the nodes can be placed into a priority queue
};

class huffman_tree {
public:
	huffman_tree(const std::string &file_name);
	~huffman_tree();

	std::string get_character_code(char character) const;
	std::string encode(const std::string &file_name) const;
	std::string decode(const std::string &string_to_decode) const;
private:
	std::map<char, int> frequencies; //Store any characters from the file in here, mapped to their frequency in the file
	std::map<char, std::string> encoded_chars; //Store the characters mapped to their codes in here
	std::priority_queue<Node*, std::vector<Node*>, Compare> node_queue; //A priority queue to make the Huffman tree
	void read_file(const std::string &file_name);
	void encode_characters(std::string code, Node* node);
	void delete_tree(Node* node);
};

#endif

