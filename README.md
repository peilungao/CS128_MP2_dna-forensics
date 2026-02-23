---
url: https://cs128.org/2026a/mp2-dna-forensics-1701
---

# [MP2] DNA Forensics

## Learning objectives and outcomes

- Opportunity to divide and conquer with functions
- More experience iterating over compound data structures and making decisions based upon observed values
- More experience with simple string parsing and basic string manipulation techniques
- Practice with simple string parsing and basic input validation
- Practice reading data from a file using input file streams
- Further practice with command line arguments

## Grading

- The assignment's final due date is shown in the yellow box at the top of this page.
- This assignment will be autograded. You have unlimited penalty-free submission attempts.
- You must correctly pass ALL test cases to earn 100 pts.
- If your code does not pass all of our test cases, these 100 pts will be prorated → 100 pts * (% what you earned / 100%).

## Submitting your work

Submit the following files to PrairieLearn for grading: `dna_forensics.cc functions.hpp functions.cc`.

- You have been allotted unlimited penalty-free submission attempts for this assignment.

## Background

DNA encodes the genetic information found in all known organisms. Using four nitrogenous bases: Adenine (A), Thymine (T), Guanine (G), and Cytosine (C), DNA encodes for different proteins that are responsible for the organism's functionality. DNA profiling, the process of determining an individual's DNA characteristics, is commonly used in forensic science, parentage tests, and medical research. However, there are over 3 billion nitrogenous bases in a typical human genome, and comparing every possible alignment to each person being profiled would be too computationally expensive. So, how do we determine which person a given DNA sequence belongs to? We leverage that most of the human genome is relatively similar, and certain areas have high genetic diversity. So, instead of matching every person's DNA to the given DNA, we can compare these highly diverse regions. These regions contain Short Tandem Repeats (STR's), short DNA sequences that repeat consecutively.
 ![michael_reese](https://cdn1.cs128.org/sp21/michael_reese.PNG)
Using multiple STRs, rather than just one, can improve the accuracy of DNA profiling. Suppose the probability that two people have the same number of repeats for a single STR is 5%, and the analyst looks at 10 different STRs. In that case, the probability that two DNA samples match purely by chance is about 1 in 1 quadrillion (assuming all STRs are independent). If two DNA samples match the number of repeats for each STR, the analyst can be confident they came from the same person.

Suppose we have 3 people (Michael, Reese, and Nathan), with STR counts for ATTA AATG and TATC. Suppose Michael has 15 consecutive occurrences of ATTA, 10 consecutive occurrences of AATG, and 12 consecutive occurrences of TATC. Similarly, Reese has 4 consecutive occurrences of ATTA, 6 consecutive occurrences of AATG, 2 consecutive occurrences of TATC and Nathan has 10 consecutive occurrences of ATTA, 4 consecutive occurrences of AATG, and 2 consecutive occurrences of TATC.

Now, suppose you're given the following DNA Strand:

```txt
ATTAATTAATTAATTAAATGAATGAATGAATGAATGAATGTATCTATCATTAAATGTATC
```

Well, imagine that you looked through the DNA sequence for the longest consecutive sequence of repeated ATTAs and found that the longest sequence was 4 repeats long. If you then find that the longest sequence of AATGs is 6 repeats long, and the longest sequence of TATC is 2 repeats long, that would provide pretty good evidence that the DNA was Reese's. **Notice that the *longest consecutive sequence* is not simply the *overall count* of the STR in the strand**.

Of course, it's also possible that once you take the counts for each of the STRs, it doesn't match anyone in your DNA database, in which case you have no match. If you were given the DNA strand:

```txt
ATTAATTAATTAATTAATTA
```

then there would be No match as none of the persons have only 5 ATTA’s in a row.

## Assignment

You will complete the DNA profiling task by implementing `ProfileDNA()`, which you will define in `functions.cc`. `ProfileDNA()` will take as arguments the absolute path to a DNA STR database (as described below) and the DNA sequence under evaluation as strings. The function will return the person's name to whom the sequence matches, or "`No match`" (case sensitive).

You must implement your solution to this problem in `ProfileDNA()`. This function *should* be written at a single level of abstraction. Therefore, use the step-down rule to create additional functions that together implement the prescribed behavior `ProfileDNA()`.

Your program will be given as command line arguments the name of a DNA database file and a string representation of the DNA sequence, we would like to determine to whom it belongs. These arguments will be used to invoke `ProfileDNA()`.

### DNA database files

DNA database files can vary from one another in the number of STRs and number of people they contain. You can assume that a The DNA database file will have at least one STR and one person present. You cannot assume that all DNA databases contain exactly the same number of STRs (e.g., 3).

To begin the analysis, your first task is to write a program that reads into memory the DNA database. In this assignment, the DNA database will be encoded as a CSV file, where each row corresponds to an individual and each column corresponds to an STR. For example,

```csv
Names,ATTA,AATG,TATC
Michael,15,10,12
Reese,4,6,2
Nathan,10,4,2
```

The DNA database encoded as a CSV file in this manner communicates three important pieces of information. First, the STRs that we will be using in our analysis (ATTA,AATG,TATC). Second, the names (Michael, Reese, Nathan) of the individuals involved in our study. Finally, the number of times each individual has a specific STR repeated consecutively in her/his DNA. Michael has ATTA repeated 15 times consecutively somewhere in his DNA, AATG 10 times, and TATC 12 times. Reese has ATTA repeated 4 times consecutively somewhere in his DNA, AATG 6 times, and TATC 2 times. Nathan has ATTA repeated 10 times consecutively somewhere in his DNA, AATG 4 times, and TATC 2 times.

All information stored in the DNA database must be read into memory. We recommend that you read the database line-by-line into your program using [std::getline](https://en.cppreference.com/w/cpp/string/basic_string/getline) until there are no more lines to be read. For example, if we would like to read each line from the file `example.dat`, we could write something like this:

```cpp
std::ifstream ifs{"example.dat"};
for(std::string line; std::getline(ifs, line); line = "")
  std::cout << "line = \"" << line << '\"' << std::endl;
```

What's happening? We attempt to extract a line from the input file stream `ifs` and store it in `line`. This is the conditional of our for-statement. If this operation succeeds, we enter the loop body. That is, if a line is successfully extracted from `ifs`, we enter the loop body and (in this example) put the `line` to standard out. After each iteration of the loop body, the `line` is "reset" to the empty string, and we attempt to extract another line from `ifs`. This process continues until there are no more lines to get, at which point `getline` will fail to extract, and the conditional will evaluate to `false`, terminating the loop.

Always assume that the first row of *any* DNA database will be the column names. The first column will not always be Names — for instance, if the dataset were French, it might read Nom — though the remaining columns will always be the STRs. Therefore, in our example, `std::getline` would read Names,ATTA,AATG,TATC into a single `std::string`. To help you obtain the different "columns", we have provided you the function `utilities::GetSubstrs` that can produce a `std::vector<std::string> {"Names", "ATTA", "AATG", "TATC"}`. After reading the first row from the file, each additional row will correspond to an individual, and each column will correspond to the number of consecutive times a particular STR repeats in their DNA. Once we begin reading str counts, notice that the integer values from the file are stored in std::string form. This goes against our preference since an std::string has different behaviors than an integer. Therefore, it is advisable to convert those numbers into integer values. You should perform the conversion using `std::stoi` from the `<string>` facilities. Here's an example:

```cpp
std::string number_as_string = "8";
int number_as_int = std::stoi(number_as_string);
/* OK, since number_as_string constitutes valid int. */

std::string string_as_string = "Howdy";
int string_as_int = std::stoi(string_as_string);
/* ERROR: terminating with uncaught exception of type std::invalid_argument: stoi: no conversion. */
```

### Analyzing the DNA Sequence

Now, we direct our attention to the DNA sequence under analysis. For each of the STRs from the DNA database, you compute and record the *longest* consecutive run of repeats for that STR in the DNA sequence. Subsequently, you compare these counts to each individual's counts. Should the STR counts in the DNA sequence match exactly one person from the database, `ProfileDNA()` must return the name of the matched individual. Otherwise, `ProfileDNA()` should return `No match` (case sensitive).

### Example

Given the DNA STR database file with the following contents:

```csv
Names,AGATG,AATG,TAT
Casey,5,2,8
Amani,3,7,4
Blair,6,1,5
```

The DNA sequence

```txt
AGACGGGTTACCATGACTATTATTATTATTATTATTATTATACGTACGTACGTATGAGATGAGATGAGATGAGATGAGATGCCTCGACTTCGATCGCAATGAATGCCAATAGACAAAA
```

would map to Casey,

```txt
TATTATTATTATAACCCTGCGCGCGCGCGATCCAGCATTAGCTAGCATCAAGATGAGATGAGATGGAATTTCGAAATGAATGAATGAATGAATGAATGAATG
```

to Amani, and

```txt
GGTACAGATGGCAAAGATGAGATGAGATGGTCGTCGAGCAATCGTTTCGATAATGAATGAATGAATGAATGAATGAATGACACACGTCGATGCTAGCGGCGGATCGTATATTATAACCCCTAG
```

to No match.

## Requirements

- Accept the assignment via the "GitHub Classroom" green button at the top of this page, and clone the repository into your development environment.
- You will perform the DNA profiling task in your application by implementing the behavior described above in the function signature `ProfileDNA(const std::string& dna_database,const std::string& dna_sequence)` with return type `std::string` (define in `functions.cc`).
- Each person's *name* will be unique in the DNA database: you do not need to consider duplicate names.
- Your program must consider each STR independent of the other STRs. For example, if you had a function that returned the longest consecutive sequence of an STR in a given DNA sequence (we recommend this), that function would be called once per STR!
- When looking for an STR in the strand, you cannot reuse characters for one match to create another match for the *same* STR; you cannot overlap characters for matches of the *same* STR. Accordingly, upon finding an STR match in the sequence, you would continue scanning for other matches from the character in the strand that follows directly after the characters comprising the match. For example,
  ![invalid-seq-match](https://cdn1.cs128.org/common/mp-dna-forensics/invalid-seq-match.png)
- **Kindly understand that the *longest consecutive sequence* is not simply the *overall count* of the STR in the strand**. The longest consecutive sequence is the longest *consecutive* "count" or "run" of an STR side-by-side itself in the strand. In contrast, the overall count of an STR is the total number of times the STR is in the strand.
- The first comma-separated entry on the first line of the DNA Database will always be a string, but you cannot assume that that string will always be `Names`.
- DNA database input files will vary in the number of STRs. You can assume that the DNA database input file will have at least one STR. You cannot assume that our test input files contain only three STRs.
- You should create your own STR DNA database input files and DNA strands to test your application
  - STR DNA database
           - Do not create these in Excel. Instead, create a new file in VS code while connected to your container/VM
           - Compose this STR DNA database file such that it follows the format specified in this prompt
           - Name the file using any extension (e.g., `.dat`, `.csv`, etc.); what's important is the format of the file's contents
  - DNA sequences
           - You should compose multiple DNA sequences for testing
           - We recommend having strands that place the longest consecutive sequence of the STR in different positions relative to shorter consecutive sequences of the same STR
           - Remember, it is the longest consecutive sequence of STRs that matters
- Compile your source code to an executable named `exec` in the `bin` folder. We've configured a workflow to do this for you:
  - In the terminal window, from the project's root directory, execute `make exec`. Provided compilation succeeds, you will find an executable named `exec` in the `bin` folder.
  - After you've made changes and are ready to compile again, we suggest executing `make clean` before `make exec` to delete the older (and not outdated) executables.
  - To run the program from the command line, ensure you're in the project's root directory, then execute `./bin/exec "arg1" "arg2"`, where `arg1` is the absolute path to the DNA STR database and `arg2` is the DNA sequence under evaluation.
- Program Usage
  - Your program must accept as command-line arguments the absolute path to the DNA STR database and the DNA sequence to analyze.
  - Your program will write the string returned by `ProfileDNA()` to standard output and subsequently terminate.
  - Note: if you're running your program with our visual debugger, please provide the absolute path to the input file. For example, if you're developing in `~/src/mp-dna-forensics-michaelrnowak/` and your test case is in `./tests/input-test-1.dat` relative to that directory, the absolute path to `input-test-1.dat` would look something *similar* to `/home/vagrant/src/mp-dna-forensics-michaelrnowak/tests/input-test-1.dat`. The absolute path to the `tests` directory could be found by `cd` into `tests` and executing `pwd`.
- Your program must compile without warnings/errors when compiled with: `clang++` using the `-std=c++20` and the following flags `-Wall -Wextra -Werror -pedantic`
- We've also included the PrairieLearn test cases in the starter code repository for this MP. We will not provide our test cases on subsequent MPs (for real, this time!). To compile your solution against the test suite,
  - In the terminal window, from the project's root directory, execute `make tests`. Provided compilation succeeds, you will find an executable named `tests` in the `bin` folder.
  - run the test suite by executing `./tests` from the project's root directory.
- Academic IntegrityYou must derive/generate your solution using string manipulation, iteration, and other language features.
  - We provide you with almost all the test cases we will use during grading.
  - Some students may be tempted to circumvent the spirit of this assignment by mapping the input to outputs directly.
           - Doing this will violate academic integrity and will result in a FAIR referral.

## Important: Use LF (Linux) Line Endings

To ensure your code works correctly with our tests, your test files **must use LF line endings**, not Windows' CRLF. To confirm that your test files are using LF endings, in Visual Studio Code, check the bottom-right corner of the window. You should see either LF or CRLF.

- **If it says CRLF:** Click it and select **LF** to change the encoding.
- **If it says LF:** Your file is correctly configured.

Please make this change for all your input files.

## Constraints

Only the following header files are allowed to be `#include`d in your solution files:
 `<cassert> <fstream> <iostream> <map> <stdexcept> <string> <vector> "functions.hpp" "utilities.hpp"`

## Hints

- Be sure to define helper functions to facilitate the comparisons, etc. otherwise you will end up with spaghetti code
- Look into `std::stoi` from `<string>` to convert strings to integers.
- `No match` happens when none of the people matches the DNA or when more than one person matches the DNA sequence under evaluation.

Components of this MP were inspired by [this](http://nifty.stanford.edu/2020/dna/) NIFTY assignment.
