
#include "functions.hpp"
#include "utilities.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <map>

int main(int argc, char* argv[]) {
  if (argc != 3) return 1;
  // (0) cultivate arguments, open dna database
  const std::string kDatabaseDNA = argv[1];
  const std::string kSequenceDNA = argv[2];
  std::cout << ProfileDNA(kDatabaseDNA, kSequenceDNA);

  // const std::string dna_dataset_path = "/Users/veritas/scholar/University/Year1-Semester2/CS_128/MP2_dna-forensics/tests/dna_databases/test-strs-3.dat";

  // const std::string dna_sequence = "CACACACACACACACACACACACAGGCATAGTTCGAGGAGAAATATCCGGGGCCCACCCACACACACACAATTAATTAATTAAAAGACAACCAGCATATTAATTAATTAATTAATTACTCGGGTCTTGCCCAACCCGATTAATTAATTAATTA";

  // std::vector<std::string> STR_list = GetSTRList(dna_dataset_path);

  // std::map<std::string, std::map<std::string, unsigned int>> dna_dataset = ReadDNADataset(dna_dataset_path);

  // std::pair<std::string, unsigned int> STR = MatchEntireSTR(0, dna_dataset_path, dna_sequence);

  // std::map<std::string, unsigned int> sequenced_dna = SequenceDNA(dna_dataset_path, dna_sequence);

  // std::string name = ProfileDNA(dna_dataset_path, dna_sequence);
  return 0;
}
