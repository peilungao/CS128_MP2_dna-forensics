#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <string>
#include <map>

#include "utilities.hpp"

std::vector<std::string> GetSTRList(std::string dna_dataset_path);

std::map<std::string, std::map<std::string, unsigned int>> ReadDNADataset(std::string dna_dataset_path);

std::map<std::string, unsigned int> InitializeDNASequence(std::string dna_dataset_path);

std::vector<std::string> SequenceCurrentDNA(
    char current_DNA,
    unsigned int DNA_index_at_STR,
    std::vector<std::string> current_match_STR_list);

std::pair<std::string, unsigned int> MatchEntireSTR(
    unsigned int starting_DNA_index,
    std::string dna_database_path,
    std::string dna_sequence);

std::map<std::string, unsigned int> CompareLongestRepeatInSTR(
    std::map<std::string, unsigned int> longest_consecutive_repeat,
    std::string current_STR,
    unsigned int current_repeat);

std::map<std::string, unsigned int> SequenceDNA(std::string dna_dataset_path,
                                                std::string dna_sequence);

std::string ProfileDNA(const std::string& dna_database,
                       const std::string& dna_sequence);

#endif
