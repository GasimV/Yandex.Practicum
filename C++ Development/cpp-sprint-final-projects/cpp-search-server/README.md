# cpp-search-server
Финальный проект: поисковый сервер

**Workflow of the code, function by function, to understand how it operates.**

### 1. **`SplitIntoWords` Function**

#### Purpose:
This function splits a given string `text` into individual words based on spaces.

#### Workflow:
- Initialize an empty `vector<string>` named `words` to store the words.
- Traverse each character in the input string `text`.
- If a space is encountered, the current word is added to the `words` vector, and the word is cleared.
- If the character is not a space, it's added to the current `word`.
- After the loop, if there's any remaining `word`, it's added to the `words` vector.
- The function returns the `words` vector containing all the split words.

### 2. **`Document` Struct**

#### Purpose:
Represents a document with three properties: `id`, `relevance`, and `rating`.

#### Workflow:
- The struct contains a default constructor and a parameterized constructor.
- It stores document information, which will be used during search operations.

### 3. **`MakeUniqueNonEmptyStrings` Template Function**

#### Purpose:
Creates a set of unique, non-empty strings from a given container (e.g., vector).

#### Workflow:
- Iterate through each string in the input `StringContainer`.
- If a string is non-empty, it's added to the `set<string>`.
- The function returns the set of unique non-empty strings.

### 4. **`DocumentStatus` Enum**

#### Purpose:
Represents the possible statuses of a document (`ACTUAL`, `IRRELEVANT`, `BANNED`, `REMOVED`).

#### Workflow:
- Used to categorize documents based on their status, which is essential for filtering during search operations.

### 5. **`SearchServer` Class**

#### Purpose:
Manages documents and performs searches based on queries. It supports adding documents, searching for top documents, and matching documents with queries.

#### Key Components and Functions:

#### **a. `SearchServer` Constructors:**
   - **Constructor with `StringContainer`:** Initializes the server with a set of stop words from the provided container.
   - **Constructor with `string`:** Initializes the server by splitting the stop words text into individual stop words and then calling the first constructor.

#### **b. `AddDocument` Function:**
   - **Purpose:** Adds a document to the search server.
   - **Workflow:**
     1. Checks if the document ID is valid (non-negative and not already in use).
     2. Verifies that the document does not contain any invalid words.
     3. Splits the document into words, ignoring stop words.
     4. Calculates the inverse word count and updates the word frequency for each word in the document.
     5. Stores the document's rating and status.
     6. Adds the document ID to the list of document IDs.

#### **c. `FindTopDocuments` Overloaded Functions:**
   - **Purpose:** Searches for the top documents matching the query. This function has three overloads:
     1. **With `DocumentPredicate`:** Finds documents matching the query and a custom predicate.
     2. **With `DocumentStatus`:** Filters documents by status and then searches.
     3. **With no parameters:** Searches for documents with the `ACTUAL` status.
   - **Workflow:**
     1. Parses the query to extract plus and minus words.
     2. Filters and sorts the matching documents based on relevance and rating.
     3. Returns the top documents, limited by `MAX_RESULT_DOCUMENT_COUNT`.

#### **d. `MatchDocument` Function:**
   - **Purpose:** Matches a document against a query to find all matching (and non-excluded) words.
   - **Workflow:**
     1. Checks if the document ID exists.
     2. Parses the query.
     3. Identifies all matching plus words in the document.
     4. Clears matches if any minus word is found in the document.
     5. Returns the matched words and the document's status.

#### **e. `GetDocumentCount` and `GetDocumentId` Functions:**
   - **Purpose:** Provide access to the total number of documents and the document ID at a specific index.

#### **f. **Private Helper Functions:**
   - **IsValidWord:** Checks if a word is valid (doesn't contain special control characters).
   - **IsStopWord:** Checks if a word is a stop word.
   - **SplitIntoWordsNoStop:** Splits a document's text into words, ignoring stop words.
   - **ComputeAverageRating:** Computes the average rating of a document from a vector of ratings.
   - **ParseQuery and ParseQueryWord:**
     - **ParseQuery:** Parses the raw query text into plus and minus words, validating each word.
     - **ParseQueryWord:** Processes an individual word, identifying whether it's a plus or minus word and whether it's a stop word.
   - **FindAllDocuments:** Finds all documents matching the parsed query and the given predicate.
   - **ComputeWordInverseDocumentFreq:** Computes the inverse document frequency for a word, used to calculate relevance.

### 6. **`PrintDocument` Function**

#### Purpose:
A utility function to print document information (`id`, `relevance`, `rating`) to the console.

### 7. **`main` Function**

#### Purpose:
Acts as an example usage of the `SearchServer` class, adding documents and performing search queries.

#### Workflow:
- Initializes the `SearchServer` with stop words.
- Adds several documents with various contents, statuses, and ratings.
- Performs searches with different criteria (`ACTUAL`, `BANNED`, even document IDs) and prints the results.

### Summary of Workflow:
1. **Initialization:** `SearchServer` is initialized with stop words.
2. **Document Addition:** Documents are added with specific IDs, contents, statuses, and ratings.
3. **Query Parsing:** Queries are parsed into plus and minus words, checking validity.
4. **Document Matching:** Documents are matched against the parsed queries based on the presence of plus and minus words.
5. **Top Document Search:** Top documents are retrieved, filtered by custom predicates or status, sorted by relevance and rating.
6. **Output:** Results are printed to the console.

By understanding each function's role in the workflow, you can see how documents are indexed, how queries are parsed and processed, and how search results are generated and returned. This holistic understanding will help you debug and optimize the code as needed.