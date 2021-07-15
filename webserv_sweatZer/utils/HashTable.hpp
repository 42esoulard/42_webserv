/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HashTable.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/09 10:41:04 by esoulard          #+#    #+#             */
/*   Updated: 2021/07/15 13:23:58 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

/*
** The goal is to create an efficient key-value handling algorithm.
** did this following the 42 HotRace challenge, for which i did a BST, which was not the 
** most efficient algo :(
**
** Following this tutorial https://www.geeksforgeeks.org/c-program-hashing-chaining/
** and mixing it up with this one https://www.tutorialspoint.com/cplusplus-program-to-implement-hash-tables
** Hash tables are said to be pretty fast for db handling, storing and searching.
** 
** This is a hash table tailored to our webserv storing needs.

** This particular HashTable implementation is an array of lists of Pairs (string, list of strings).
**
** It is implemented to parse a char* line such as
**          key             val
** (a unique key followed by one value, spaces are ignored)
**
** If we try to add a key that already exists, the preexisting pair isn't updated.
**
** Note: in case of collision (= if we find a same hash for different keys), we have to push back a Pair on the List of the corresponding index
**
** How get the value that matches a key:
** std::string value = hashtable.get_value(key);
** std::cout << "Hey I'm " << value << "and I'm a value for key " << key << std::endl;
*/



#include <string>
#include <iostream>
#include <list>

class Pair {

    public:

        Pair(std::string k, std::string v): _key(k), _val(v) {};

        std::string  &getKey()    { return _key; };
        std::string  &getVal()    { return _val; };

    private:

        std::string  _key;
        std::string  _val;
};

class SimpleHashTable {

    public:

        SimpleHashTable(int buckets) : _size(buckets) {
            
            _bucket_list = new std::list<Pair>[_size]();
        };// constructor

        ~SimpleHashTable() { delete[] _bucket_list; };

        long int hashFunction(std::string &key) const {
            
            long int hash = 37; // a prime number
            long int prime_a = 71; //another prime number
            
            std::string::iterator ite = key.end();
            for (std::string::iterator c = key.begin(); c != ite; ++c) {
                
                hash = (hash * prime_a + *c) % _size;
            }

            return hash;
        }

        bool is_space(char a) {
            if (a == ' ' || a == '\t'  || a == '\n'  || a == '\v'
                || a == '\f'  || a ==  '\r')
                return true;
            return false;
        }

        size_t pass_spaces(std::string &line, size_t &index) {
            
            if (line == "")
                return index;
            while (index < line.size() && is_space(line[index]))
                index++;
            return index;
        }

        // simply parses spaces and returns the next non space character sequence
        std::string get_next_token(std::string &line, size_t &index) {

            pass_spaces(line, index);
            if (line == "" || index >= line.size())
                return "";

            int start = index;
            while (index < line.size() && !is_space(line[index]))// && line[index] != ';')
                ++index;

            return line.substr(start, index - start);
        }

        void add_entry(std::string &input) {

            size_t i = 0;
            std::string key = get_next_token(input, i);
            long int    index = hashFunction(key);

            std::list<Pair>::iterator it = get_pair(key, index);
            std::list<Pair>::iterator ite = get_index_end(index);

            if (it != ite && it->getKey() == key) 
                return ;

            std::string value = get_next_token(input, i);
            
            _bucket_list[index].push_back(Pair(key, value));

        }

        std::string get_next_token_w_spaces(std::string &line, size_t &index) {

            pass_spaces(line, index);
            if (line == "" || index >= line.size())
                return "";

            int start = index;
            while (index < line.size())// && line[index] != ';')
                ++index;

            return line.substr(start, index - start);
        }

        void add_entry_w_spaces(std::string &input) {

            size_t i = 0;
            std::string key = get_next_token(input, i);
            long int    index = hashFunction(key);
            
            std::list<Pair>::iterator it = get_pair(key, index);
            std::list<Pair>::iterator ite = get_index_end(index);

            if (it != ite && it->getKey() == key) 
                return ;

            std::string value = get_next_token_w_spaces(input, i);
            
            _bucket_list[index].push_back(Pair(key, value));

        }

        std::string *get_value(std::string &key) const {

            long int                    index = hashFunction(key);

            std::list<Pair>::iterator   it;
            std::list<Pair>::iterator   ite = _bucket_list[index].end();

            if (_bucket_list[index].size()) {
                
                it = _bucket_list[index].begin();
                while (it != ite) {
                    if ((*it).getKey() == key)
                        break;
                    ++it;
                }
            }
            else
                return NULL;
            if (it == ite)
                return NULL;
    
            return &it->getVal();
        }

        std::list<Pair>::iterator get_index_end(long int index) {

            return _bucket_list[index].end();
        }

        std::list<Pair>::iterator get_pair(std::string &key, long int index) {

            std::list<Pair>::iterator it;
            std::list<Pair>::iterator ite = _bucket_list[index].end();

            if (_bucket_list[index].size()) {

                it = _bucket_list[index].begin();
                while (it != ite) {
                    if ((*it).getKey() == key)
                        break;
                    ++it;
                }
            }
            else
                return ite;
    
            return it;
        }

        // void print_entry(std::string &key) {

        //     long int index = hashFunction(key);    
        //     std::list<Pair>::iterator it = get_entry(key, index);
        
        //     if (it != _bucket_list[index].end())
        //         std::cout << (*it).getVal() << "\n";
        //     else
        //         std::cout << key << ": Not found\n";
            
            
        // };

        void delete_entry(std::string &key) {
            
            std::string toDel = key;
            long int index = hashFunction(toDel);
            std::list<Pair>::iterator it = get_pair(toDel, index); 
            
            if (it == _bucket_list[index].end())
                return ;

            _bucket_list[index].erase(it);
        }

    private:

        // array containing buckets, each bucket is supposed to hold only one value but can hold a few if theres hash collision
        std::list<Pair>     *_bucket_list;
        int                 _size;


};

// int main() {

//     HashTable                   db(SIZE);
//     std::string                 in_str;
//     std::string::iterator       c;
//     size_t                      equal;

//     // std::ios_base::sync_with_stdio(false);
//     // std::cin.tie(NULL);

//     while (1) {

//         std::getline(std::cin, in_str);
//         if (!std::cin)
//             break ;

//         c = in_str.begin();
        
//         if (*c == '!') 
//             db.delete_entry(in_str); 
//         else if ((equal = in_str.find('=')) != std::string::npos)
//             db.add_entry(in_str, equal);
//         else
//             db.print_entry(in_str);

//         in_str.clear();
//     }
//     return 0;
// }





/*
** BELOW IS A VERSION WITH A LIST OF STRINGS AS VALUE
** It's gonna be less efficient but useful in case we often need to store several values for a same key 
*/


/*
** This particular HashTable implementation is an array of lists of Pairs (string, list of strings).
**
** It is implemented to parse a char* line such as
**          key             val     val val
** (a key followed by one or several values, spaces are ignored)
**
** If we have several values for a same key, we add it in the value list
** the value getter returns a list<std::string>, so we can iterate on this list from outside
**
** Note: in case of collision (= if we find a same hash for different keys), we have to push back a Pair on the List of the corresponding index
**
** How to explore all the values for a key:
** std::list<std::string> values = get_value(key);
** std::list<std::string>::iterator it = values.begin();
** while (it != values.end())
**      std::cout << "Hey I'm " << *it << "and I'm a value for key " << key << std::endl;
*/

// #include <string>
// #include <iostream>
// #include <list>

// class Pair {

//     public:

//         Pair(std::string k, std::string v): _key(k) { addVal(v); };

//         std::string             &getKey()    { return _key; };
//         std::list<std::string>  &getVal()    { return _val; };
    
//         void                    addVal(std::string v) { _val.push_back(v); }  

//     private:

//         std::string             _key;
//         std::list<std::string>  _val;
// };

// class HashTable {

//     public:

//         HashTable(int buckets) : _size(buckets) {
            
//             _bucket_list = new std::list<Pair>[_size];
//         };// constructor

//         ~HashTable() {};

//         long int hashFunction(std::string &key) const {
            
//             long int hash = 37; // a prime number
//             long int prime_a = 71; //another prime number
            
//             std::string::iterator ite = key.end();
//             for (std::string::iterator c = key.begin(); c != ite; ++c)
//                 hash = (hash * prime_a + *c) % _size;

//             return hash;
//         }

//         bool is_space(char a) {
//             if (a == ' ' || a == '\t'  || a == '\n'  || a == '\v'
//                 || a == '\f'  || a ==  '\r')
//                 return true;
//             return false;
//         }

//         int pass_spaces(char *line, int &index) {
            
//             if (!line)
//                 return index;
//             while (line && line[index] && is_space(line[index]))
//                 index++;
//             return index;
//         }

//         // simply parses spaces and returns the next non space character sequence
//         std::string get_next_token(char *line, int &index) {

//             pass_spaces(line, index);
//             if (!line || !line[index])
//                 return "";

//             int start = index;
//             while (line && line[index] && !is_space(line[index]))// && line[index] != ';')
//                 ++index;

//             return std::string(&line[start], index - start);
//         }

//         void add_entry(char *input) {

//             int i = 0;
//             std::string key = get_next_token(input, i);
//             long int    index = hashFunction(key);

//             std::list<Pair>::iterator it = get_pair(key, index);
//             std::list<Pair>::iterator ite = get_index_end(index);

//             std::string value = get_next_token(input, i);
//             if (it == ite) {
//                 _bucket_list[index].push_back(Pair(key, value));
//                 it = get_pair(key, index);
//             }

//             while ((value = get_next_token(input, i)) != "")
//                 it->addVal(value);

//         }

//         std::list<std::string> *get_values(std::string &key) const {

//             long int                    index = hashFunction(key);

//             std::list<Pair>::iterator   it;
//             std::list<Pair>::iterator   ite = _bucket_list[index].end();

//             if (_bucket_list[index].size()) {
                
//                 it = _bucket_list[index].begin();
//                 while (it != ite) {
//                     if ((*it).getKey() == key)
//                         break;
//                     ++it;
//                 }
//             }
//             else
//                 return NULL;
//             if (it == ite)
//                 return NULL;
    
//             return &(it->getVal());
//         }

//         std::list<Pair>::iterator get_index_end(long int index) {

//             return _bucket_list[index].end();
//         }

//         std::list<Pair>::iterator get_pair(std::string &key, long int index) {

//             std::list<Pair>::iterator it;
//             std::list<Pair>::iterator ite = _bucket_list[index].end();

//             if (_bucket_list[index].size()) {

//                 it = _bucket_list[index].begin();
//                 while (it != ite) {
//                     if ((*it).getKey() == key)
//                         break;
//                     ++it;
//                 }
//             }
//             else
//                 return ite;
    
//             return it;
//         }

//         // void print_entry(std::string &key) {

//         //     long int index = hashFunction(key);    
//         //     std::list<Pair>::iterator it = get_entry(key, index);
        
//         //     if (it != _bucket_list[index].end())
//         //         std::cout << (*it).getVal() << "\n";
//         //     else
//         //         std::cout << key << ": Not found\n";
            
            
//         // };

//         void delete_entry(std::string &key) {
            
//             std::string toDel = key;
//             long int index = hashFunction(toDel);
//             std::list<Pair>::iterator it = get_pair(toDel, index); 
            
//             if (it == _bucket_list[index].end())
//                 return ;

//             _bucket_list[index].erase(it);
//         }

//     private:

//         // array containing buckets, each bucket is supposed to hold only one value but can hold a few if theres hash collision
//         std::list<Pair>     *_bucket_list;
//         int                 _size;


// };

// // int main() {

// //     HashTable                   db(SIZE);
// //     std::string                 in_str;
// //     std::string::iterator       c;
// //     size_t                      equal;

// //     // std::ios_base::sync_with_stdio(false);
// //     // std::cin.tie(NULL);

// //     while (1) {

// //         std::getline(std::cin, in_str);
// //         if (!std::cin)
// //             break ;

// //         c = in_str.begin();
        
// //         if (*c == '!') 
// //             db.delete_entry(in_str); 
// //         else if ((equal = in_str.find('=')) != std::string::npos)
// //             db.add_entry(in_str, equal);
// //         else
// //             db.print_entry(in_str);

// //         in_str.clear();
// //     }
// //     return 0;
// // }

#endif