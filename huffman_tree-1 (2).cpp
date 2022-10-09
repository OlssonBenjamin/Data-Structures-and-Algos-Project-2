#include "huffman_tree.h"

/*
Preconditions: file_name is the name of (and possibly path to) a text file
Postconditions: Reads the contents of file_name and constructs a
				huffman tree based on the character frequencies of the file contents
*/
huffman_tree::huffman_tree(const std::string &file_name) {
	read_file(file_name);
	for (auto it = frequencies.begin(); it != frequencies.end(); it++) {
		if (it->second > 0) {
			Node* node = new Node(it->first, it->second); //Each node created here is a leaf node, it has a valid character
			node_queue.push(node);
		}
	}
	while (node_queue.size() > 1) {
		Node* node = new Node(); //Each node created here is not a leaf node, it doesn't have a valid ascii value and thus won't be read
		Node* left = node_queue.top();
		node_queue.pop();
		Node* right = node_queue.top();
		node_queue.pop();
		node->frequency = left->frequency + right->frequency;
		node->left = left;
		node->right = right;
		node_queue.push(node);
	}
	if (!node_queue.empty() && frequencies.size() != 1)
		encode_characters("", node_queue.top());
	if (frequencies.size() == 1) { //If there is only one character, than there is only one encoding, "0"
		auto it = frequencies.begin();
		encoded_chars.emplace(it->first, "0");
	}
}

huffman_tree::~huffman_tree() {
    if (!node_queue.empty()) { //Don't attempt to access empty queue
        Node* node = node_queue.top();
        node_queue.pop();
        delete_tree(node);
    }
}

/*
Preconditions: Character is a character with an ASCII value
				between 0 and 127 (inclusive).
Postconditions: Returns the Huffman code for character if character is in the tree
				and an empty string otherwise.
*/
std::string huffman_tree::get_character_code(char character) const {
	auto it = encoded_chars.find(character);
	if (it == encoded_chars.end())
		return "";
	else
		return it->second;
}

/*
Preconditions: file_name is the name of (and possibly path to) a text file
Postconditions: Returns the Huffman encoding for the contents of file_name
				if file name exists and an empty string otherwise.
				If the file contains letters not present in the huffman_tree,
				return an empty string
*/
std::string huffman_tree::encode(const std::string &file_name) const {
	std::string encoded;
	std::ifstream file(file_name);
	if (file.is_open()) {
		std::string line;
		char character;
		while (std::getline(file, line)) {
			for (unsigned int i = 0; i < line.size(); i++) {
				character = line[i];
				auto it = encoded_chars.find(character);
				if (it == encoded_chars.end())
					return "";
				else
					encoded += it->second;
			}
			if (file.good()) {
				auto it = encoded_chars.find('\n');
				if (it != encoded_chars.end())
					encoded += it->second;
				else
					return "";
			}
		}
		file.close();
	}
	else
		return "";
	return encoded;
}

/*
Preconditions: string_to_decode is a string containing Huffman-encoded text
Postconditions: Returns the plaintext represented by the string if the string
				is a valid Huffman encoding and an empty string otherwise
*/
std::string huffman_tree::decode(const std::string &string_to_decode) const {
	std::string decoded;
	if (frequencies.size() > 1) {
		unsigned int i = 0;
		while (i < string_to_decode.size()) {
            //I need to start back at the top of the tree after finding each character
			Node* node = node_queue.top();
            //Traverse the tree according to the characters in the string
			while (node != nullptr && node->character < 0 && i < string_to_decode.size()) {
				char bit = string_to_decode[i];
				if (bit == '0') {
					node = node->left;
					i++;
				}
				else if (bit == '1') {
					node = node->right;
					i++;
				}
				else //If something other than a 0 or 1 is read, return an empty string as that is not a valid encoding
					return "";
			}
			if (node != nullptr)
				decoded += node->character;
			else //If the node is a nullptr, then an invalid code was passed in
				return "";
		}
	}
	else if (frequencies.size() == 1) {
		auto it = frequencies.begin();
		char only_char = it->first;
		for (unsigned int i = 0; i < string_to_decode.size(); i++) {
			char decode = string_to_decode[i];
			if (decode == '0')
				decoded += only_char;
			else
				return "";
		}
	}
    else {
        return "";
    }
	return decoded;
}

//Helper structs and functions

Node::Node(char character_, int frequency_) {
	character = character_;
	frequency = frequency_;
	right = nullptr;
	left = nullptr;
}

bool Compare::operator()(const Node* left, const Node* right) const {
	return (left->frequency > right->frequency); //Use > so that the priority queue has the smallest frequencies at the top
}

void huffman_tree::read_file(const std::string &file_name) {
	std::ifstream file(file_name);
	if (file.is_open()) 
	{
		std::string line;
		char character;
		int numLines = 0; //Stores the number of newline characters in the file, as getline uses newline as its default delimiter, so it won't read newlines
		while(std::getline(file, line)) {
			for (unsigned int i = 0; i < line.size(); i++) {
				character = line[i];
				auto it = frequencies.find(character);
				if (it == frequencies.end()) //Find returns an iterator pointing to the character, int pair if the character has already been put into the map, otherwise it points to end(), so a new character has been reached
					frequencies.emplace(character, 1);
				else
					it->second += 1;
			}
			if (file.good()) //If the end of the file hasn't been reached, then getline ran into a newline
				numLines++;
		}
		if (numLines > 0)
			frequencies.emplace('\n', numLines); //Emplace the positive number of newline characters that were found
		file.close();
	}
}

void huffman_tree::encode_characters(std::string code, Node* node) {
    //This function finds the encoding of each character and places it into a map, so files can be more quickly encoded
	if (node->character >= 0) //If the node has a character greater than zero, than it is a leaf node storing an ascii character, so store that character along with the current string
		encoded_chars.emplace(node->character, code);
	else { //Else, continue traversing down the tree, adding a zero if going to the left child and a 1 if going to the right child
		if (node->left != nullptr) {
			std::string temp = code;
			temp += "0";
			encode_characters(temp, node->left);
		}
		if (node->right != nullptr) {
			std::string temp = code;
			temp += "1";
			encode_characters(temp, node->right);
		}
	}
	return;
}

void huffman_tree::delete_tree(Node* node) {
    //In order to delete the entire tree, you must go from the bottom up, otherwise you will lose the pointers to the rest of the nodes
	if (node->left != nullptr)
		delete_tree(node->left);
	if (node->right != nullptr)
		delete_tree(node->right);
	if (node != nullptr)
		delete node;
	return;
}
