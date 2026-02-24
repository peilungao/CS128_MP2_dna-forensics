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
  std::vector<std::string> str_list;
  unsigned int current_line_index = 0;
  for (std::string line; std::getline(ifs, line); line = "") {
    // std::cout << "line = \"" << line << '\"' << std::endl;
    std::vector<std::string> data = utilities::GetSubstrs(line, ',');
    // The first comma-separated entry on the first line of the DNA Database
    // will always be a string, but you cannot assume that that string will
    // always be `Names`.
    if (current_line_index == 0) {                      // First row
      for (unsigned int i = 1; i < data.size(); i++) {  // STR
        str_list.push_back(data[i]);
      }
    }
    current_line_index++;
  }
  return str_list;
}

/**
 * @param dna_database_path the absolute path to a DNA STR database
 * @return dna_dataset, like {Casey: {AGATG:5}, {AATG:2}, {TAT:8}}
 */
std::map<std::string, std::map<std::string, unsigned int>> ReadDNADataset(
    std::string dna_database_path) {
  std::vector<std::string> str_list = GetSTRList(dna_database_path);
  std::ifstream ifs{dna_database_path};
  std::map<std::string, std::map<std::string, unsigned int>>
      dna_dataset;  // Maps name into corresponding STR sequence
  unsigned int current_line_index = 0;
  for (std::string line; std::getline(ifs, line); line = "") {
    // std::cout << "line = \"" << line << '\"' << std::endl;
    std::vector<std::string> data = utilities::GetSubstrs(line, ',');
    std::string first_col = data[0];
    if (current_line_index != 0) {  // Data rows
      std::map<std::string, unsigned int> str_map;
      for (unsigned int i = 1; i < data.size();
           i++) {  // Occurrence of STR in each person
        unsigned int occurrence =
            static_cast<unsigned int>(std::stoul(data[i]));
        str_map.insert(std::make_pair(str_list[i - 1], occurrence));
      }
      dna_dataset.insert(std::make_pair(first_col, str_map));
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
  std::vector<std::string> str_list = GetSTRList(dna_database_path);
  std::map<std::string, unsigned int> initial_map;
  for (unsigned int i = 0; i < str_list.size(); i++) {  // Initialize the map
    initial_map.insert(std::make_pair(str_list[i], 0));
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
    char current_dna,
    unsigned int dna_index_at_str,
    std::vector<std::string> current_match_str_list) {
  std::vector<std::string> new_match_str_list;
  for (auto& str : current_match_str_list) {
    if (str[dna_index_at_str] == current_dna) {
      new_match_str_list.push_back(str);
    }
  }
  return new_match_str_list;
}

/**
 * @param starting_DNA_index unsigned int
 * @param dna_database_path the absolute path to a DNA STR database
 * @param dna_sequence the DNA sequence under evaluation as strings
 * @return current_match: the current STR matched, ending_DNA_index: the index of which next match shall start
 * Matches the next STR in the dna_sequence
 */
std::pair<std::string, unsigned int> MatchEntireSTR(
    unsigned int starting_dna_index,
    std::string dna_database_path,
    std::string dna_sequence) {
  unsigned int ending_dna_index = starting_dna_index;
  std::vector<std::string> str_list = GetSTRList(dna_database_path);
  std::vector<std::string> new_match_str_list = str_list;
  bool is_match = false;
  unsigned int dna_index_at_str = 0;
  bool success_match = false;
  while (!success_match && ending_dna_index + dna_index_at_str < dna_sequence.size()) {
    char dna = dna_sequence[ending_dna_index + dna_index_at_str];
    new_match_str_list =
        SequenceCurrentDNA(dna, dna_index_at_str, new_match_str_list);
    if (is_match) {
      if (new_match_str_list.empty()) {
        // Restart the process, starting from the next character
        is_match = false;
        new_match_str_list = str_list;
        ending_dna_index++;  // where a bug came out, previously it's ending_dna_index+=dna_index_at_str
        dna_index_at_str = 0;
      } else {
        // Match success
        success_match = new_match_str_list.size() == 1 &&
                        new_match_str_list[0].size() == dna_index_at_str + 1;
      }
      if (success_match) {
        // return the matched STR
        std::string str_matched = new_match_str_list[0];
        unsigned int new_starting_index = ending_dna_index + dna_index_at_str + 1;
        return std::make_pair(str_matched, new_starting_index);
      } else if (is_match) {
        dna_index_at_str++;
      }
    } else if (!new_match_str_list.empty()) {
      is_match = true;
      dna_index_at_str++;
    } else {
      new_match_str_list = str_list;
      ending_dna_index++;
    }
  }
  return std::make_pair("", dna_sequence.size() + 1);
}

/**
 * @param longest_consecutive_repeat std::map<std::string, unsigned int>
 * @param current_STR std::string
 * @param current_repeat unsigned int
 */
std::map<std::string, unsigned int> CompareLongestRepeatInSTR(
    std::map<std::string, unsigned int> longest_consecutive_repeat,
    std::string current_str,
    unsigned int current_repeat) {
  std::map<std::string, unsigned int> new_consecutive_repeat = longest_consecutive_repeat;
  if (!current_str.empty()) {
    unsigned int existing_repeat = new_consecutive_repeat[current_str];
    if (current_repeat > existing_repeat) {
      new_consecutive_repeat[current_str] = current_repeat;
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
  std::map<std::string, unsigned int> longest_consecutive_repeat = InitializeDNASequence(dna_database_path);
  unsigned int dna_index = 0;
  std::string current_str_match = "SAD";
  unsigned int current_str_repeat = 1;
  while (dna_index < dna_sequence.size()) {
    std::pair<std::string, unsigned int> matched_str = MatchEntireSTR(dna_index, dna_database_path, dna_sequence);
    std::string str = matched_str.first;
    unsigned int difference = matched_str.second - dna_index;
    dna_index = matched_str.second;
    if (str == current_str_match) {
      if (difference != str.size()) {
        current_str_repeat = 0;
      }
      current_str_repeat++;
    } else {
      current_str_match = str;
      current_str_repeat = 1;
    }
    longest_consecutive_repeat = CompareLongestRepeatInSTR(
        longest_consecutive_repeat, current_str_match, current_str_repeat);
  }
  longest_consecutive_repeat = CompareLongestRepeatInSTR(
      longest_consecutive_repeat, current_str_match, current_str_repeat);

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
  unsigned int match_person_count = 0;
  for (const auto& [person, str_fingerprint] : dna_dataset) {
    bool person_match = true;
    for (const auto& [str, longest_repeat] : sequenced_result) {
      // Check through all STR profile
      if (str_fingerprint.contains(str)) {
        if (str_fingerprint.at(str) != longest_repeat) {
          person_match = false;
        }
      }
    }
    if (person_match) {
      person_name = person;
      match_person_count++;
    }
  }
  if (match_person_count > 1) {
    person_name = "No match";
  }

  return person_name;
}
