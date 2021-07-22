/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HashTable.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esoulard <esoulard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/09 10:41:04 by esoulard          #+#    #+#             */
/*   Updated: 2021/07/22 16:32:36 by esoulard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

/*
** This is a hash table tailored to our webserv storing needs.
** This particular HashTable implementation is an array of lists of Pairs (string, list of strings).
**
** It is implemented to parse a char* line such as
**          key             val
** (a unique key followed by one value, spaces are ignored)
**
** If we try to add a key that already exists, the preexisting pair isn't updated.
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
        };

        SimpleHashTable() {};
        ~SimpleHashTable() { delete[] _bucket_list; };

        long int hashFunction(std::string &key) const {
            
            long int hash = 37; // random prime number
            long int prime_a = 71; //random prime number
            
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
            while (index < line.size())
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

#endif