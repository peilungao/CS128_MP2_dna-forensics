#include "functions.hpp"
#include "utilities.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <map>

/**
 * @param dna_database_path the absolute path to a DNA STR database
 * @return STR_list, like [AGATG, AATG, TAT]
 */
std::vector<std::string> GetSTRList(std::string dna_database_path) {
  std::ifstream ifs{dna_database_path};
  std::vector<std::string> STR_list;
  unsigned int current_line_index = 0;
  for (std::string line; std::getline(ifs, line); line = "") {
    std::cout << "line = \"" << line << '\"' << std::endl;
    std::vector<std::string> data = utilities::GetSubstrs(line, ',');
    std::string first_col = data[0];
    // The first comma-separated entry on the first line of the DNA Database
    // will always be a string, but you cannot assume that that string will
    // always be `Names`.
    if (current_line_index == 0) {                      // First row
      for (unsigned int i = 1; i < data.size(); i++) {  // STR
        STR_list.push_back(data[i]);
      }
    }
    current_line_index++;
  }
  return STR_list;
}

/**
 * @param dna_database_path the absolute path to a DNA STR database
 * @return dna_dataset, like {Casey: {AGATG:5}, {AATG:2}, {TAT:8}}
 */
std::map<std::string, std::map<std::string, unsigned int>> ReadDNADataset(
    std::string dna_database_path) {
  std::vector<std::string> STR_list = GetSTRList(dna_database_path);
  std::ifstream ifs{dna_database_path};
  std::map<std::string, std::map<std::string, unsigned int>>
      dna_dataset;  // Maps name into corresponding STR sequence
  unsigned int current_line_index = 0;
  for (std::string line; std::getline(ifs, line); line = "") {
    std::cout << "line = \"" << line << '\"' << std::endl;
    std::vector<std::string> data = utilities::GetSubstrs(line, ',');
    std::string first_col = data[0];
    if (current_line_index != 0) {  // Data rows
      std::map<std::string, unsigned int> STR_map;
      for (unsigned int i = 1; i < data.size();
           i++) {  // Occurrence of STR in each person
        unsigned int occurrence = std::stoi(data[i]);
        STR_map.insert(std::make_pair(STR_list[i - 1], occurrence));
      }
      dna_dataset.insert(std::make_pair(first_col, STR_map));
    }
    current_line_index++;
  }
  return dna_dataset;
}

/**
 * @param dna_database_path the absolute path to a DNA STR database
 * @return the initial map with all entries in 0{STR: STR_list, 0}
 */
std::map<std::string, unsigned int> InitializeDNASequence(
    std::string dna_database_path) {
  std::vector<std::string> STR_list = GetSTRList(dna_database_path);
  std::map<std::string, unsigned int> initial_map;
  for (unsigned int i = 0; i < STR_list.size(); i++) {  // Initialize the map
    initial_map.insert(std::make_pair(STR_list[i], 0));
  }
  return initial_map;
}

/**
 * @param current_DNA char
 * @param DNA_index_at_STR unsigned int
 * @param current_match_STR_list std::vector<std::string>
 * @return new_match_STR_list std::vector<std::string>
 */
std::vector<std::string> SequenceCurrentDNA(
    char current_DNA,
    unsigned int DNA_index_at_STR,
    std::vector<std::string> current_match_STR_list) {
  std::vector<std::string> new_match_STR_list;
  for (auto& STR : current_match_STR_list) {
    if (STR[DNA_index_at_STR] == current_DNA) {
      new_match_STR_list.push_back(STR);
    }
  }
  return new_match_STR_list;
}

/**
 * @param starting_DNA_index unsigned int
 * @param dna_database_path the absolute path to a DNA STR database
 * @param dna_sequence the DNA sequence under evaluation as strings
 * @return current_match, ending_DNA_index
 */
std::pair<std::string, unsigned int> MatchEntireSTR(
    unsigned int starting_DNA_index,
    std::string dna_database_path,
    std::string dna_sequence) {
  unsigned int ending_DNA_index = starting_DNA_index;
  std::vector<std::string> STR_list = GetSTRList(dna_database_path);
  std::vector<std::string> new_match_STR_list = STR_list;
  bool is_match = false;
  unsigned int DNA_index_at_STR = 0;
  bool success_match = false;
  while (!success_match && ending_DNA_index + DNA_index_at_STR < dna_sequence.size()) {
    char DNA = dna_sequence[ending_DNA_index + DNA_index_at_STR];
    new_match_STR_list = SequenceCurrentDNA(DNA, DNA_index_at_STR, new_match_STR_list);
    if (is_match) {
      if (new_match_STR_list.empty()) {
        // Restart the process, starting from the next character
        is_match = false;
        new_match_STR_list = STR_list;
        ending_DNA_index++;  // where a bug came out
        DNA_index_at_STR = 0;
      } else {
        // Match success
        success_match = new_match_STR_list.size() == 1 && new_match_STR_list[0].size() == DNA_index_at_STR + 1;
      }
      if (success_match) {
        // return the matched STR
        std::string STR_matched = new_match_STR_list[0];
        unsigned int new_starting_index = ending_DNA_index + DNA_index_at_STR + 1;
        return std::make_pair(STR_matched, new_starting_index);
      } else if (is_match) {
        DNA_index_at_STR++;
      }
    } else if (!new_match_STR_list.empty()) {
      is_match = true;
      DNA_index_at_STR++;
    } else {
      new_match_STR_list = STR_list;
      ending_DNA_index++;
    }
  }
  return std::make_pair("", dna_sequence.size() + 1);
}

/**
 * @param longest_consecutive_repeat std::map<std::string, unsigned int>
 * @param current_STR std::string
 * @param current_repeat unsigned int
 */
std::map<std::string, unsigned int> CompareLongestRepeatInSTR(std::map<std::string, unsigned int> longest_consecutive_repeat, std::string current_STR, unsigned int current_repeat) {
  std::map<std::string, unsigned int> new_consecutive_repeat = longest_consecutive_repeat;
  if (current_STR != "") {
    unsigned int existing_repeat = new_consecutive_repeat[current_STR];
    if (current_repeat > existing_repeat) {
      new_consecutive_repeat[current_STR] = current_repeat;
    }
  }
  return new_consecutive_repeat;
}

/**
 * @param dna_database_path the absolute path to a DNA STR database
 * @param dna_sequence the DNA sequence under evaluation as strings
 * @return longest consecutive repeat of each target STR sequence targeted
 */
std::map<std::string, unsigned int> SequenceDNA(std::string dna_database_path,
                                                std::string dna_sequence) {
  std::vector<std::string> STR_list = GetSTRList(dna_database_path);
  std::map<std::string, unsigned int> longest_consecutive_repeat = InitializeDNASequence(dna_database_path);
  unsigned int DNA_index = 0;
  std::string current_STR_match = "SAD";
  unsigned int current_STR_repeat = 1;
  while (DNA_index < dna_sequence.size()) {
    std::pair<std::string, unsigned int> MatchedSTR = MatchEntireSTR(DNA_index, dna_database_path, dna_sequence);
    std::string STR = MatchedSTR.first;
    unsigned int difference = MatchedSTR.second - DNA_index;
    DNA_index = MatchedSTR.second;
    if (STR == current_STR_match) {
      if (difference != STR.size()) {
        current_STR_repeat = 0;
      }
      current_STR_repeat++;
    } else {
      current_STR_match = STR;
      current_STR_repeat = 1;
    }
    longest_consecutive_repeat = CompareLongestRepeatInSTR(longest_consecutive_repeat, current_STR_match, current_STR_repeat);
  }
  longest_consecutive_repeat = CompareLongestRepeatInSTR(longest_consecutive_repeat, current_STR_match, current_STR_repeat);

  return longest_consecutive_repeat;
};

/**
 * @param dna_database_path the absolute path to a DNA STR database
 * @param dna_sequence the DNA sequence under evaluation as strings
 * @return the person's name to whom the sequence matches, or "`No match`" (case
 * sensitive).
 * write your implementation here... we strongly encourage that you leverage additional functions to define this behavior.
 */
std::string ProfileDNA(const std::string& dna_database_path,
                       const std::string& dna_sequence) {
  std::cout << "Input dna_database : " << dna_database_path << std::endl;
  std::cout << "Input dna_sequence : " << dna_sequence << std::endl;
  std::map<std::string, std::map<std::string, unsigned int>> dna_dataset = ReadDNADataset(dna_database_path);
  std::map<std::string, unsigned int> sequenced_result = SequenceDNA(dna_database_path, dna_sequence);
  std::string person_name = "No match";
  for (const auto& [person, STR_fingerprint] : dna_dataset) {
    bool person_match = true;
    for (const auto& [STR, longest_repeat] : sequenced_result) {
      // Check through all STR profile
      if (STR_fingerprint.contains(STR)) {
        if (STR_fingerprint.at(STR) != longest_repeat) {
          person_match = false;
        }
      }
    }
    if (person_match) {
      person_name = person;
    }
  }

  return person_name;
}
