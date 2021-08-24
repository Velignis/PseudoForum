#include "WebayForum.h"
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

// Add post with user, title, and content
void WebayForumData::addOpPost(const std::string& user, const std::string& title, const std::string& content, const std::string& date, const std::string& time)
{
	if (!full())
	{
		// Typedefs don't work for unique pointers for no reason
		// Create new op post object
		std::unique_ptr<ForumOP> newOpPost(new ForumOP);
		newOpPost->postUser = user;
		newOpPost->postTitle = title;
		newOpPost->postContent = content;
		newOpPost->postDate = date;
		newOpPost->postTime = time;

			// Reserve more space when vector is full
			if (forumOpList.size() == forumOpList.capacity())
				reserveListSpace();
			
			forumOpList.push_back(std::move(newOpPost));

	}
} // end addOpPost

void WebayForumData::addReplyPost(const int& OpId, std::vector<int> ReplyId, const std::string& user, const std::string& content, const std::string& date, const std::string& time)
{
	if (!full())
	{
		std::shared_ptr<ForumPostNode> newReplyPost(new ForumPostNode);
		newReplyPost->postUser = user;
		newReplyPost->postContent = content;
		newReplyPost->postDate = date;
		newReplyPost->postTime = time;

		//if there is an op post at the id, has existing replies, and the reply id input is not empty
		if ((forumOpList[OpId] != nullptr) && ((!forumOpList[OpId]->replies.empty()) && (!ReplyId.empty())))
			findReply(OpId, ReplyId).first->replies.push_back(newReplyPost);

		//if there is an op post at the id, has existing replies, and the reply id input is empty
		else if((forumOpList[OpId] != nullptr) && ((!forumOpList[OpId]->replies.empty()) && ReplyId.empty()))
			forumOpList[OpId]->replies.push_back(newReplyPost);

		//if there is an op post at the id and has no replies
		else if (forumOpList[OpId] != nullptr)
			forumOpList[OpId]->replies.push_back(newReplyPost);
	}
} // end addReplyPost

// Deletes Op Post, along with any replies
void WebayForumData::deleteOpPost(const int& OpId)
{
	// if the deleted op post is at the end of the list, simply remove from list, else put on deletedOP vector
	if ((OpId == (forumOpList.size()-1)) && (forumOpList[OpId] != nullptr))
	{
		//forumOpList[OpId].reset();
		forumOpList.pop_back();
	}
	else
	{
		//forumOpList[OpId].reset(); more optimal method being to use this
		//deletedOpList.push_back(OpId);
		forumOpList.erase(forumOpList.begin() + OpId);
	}
} // end deleteOpPost

// Deletes reply, along with any replies to it
void WebayForumData::deleteReplyPost(const int& OpId, std::vector<int> ReplyId)
{
	if ((forumOpList[OpId] != nullptr) && ((!forumOpList[OpId]->replies.empty()) && (!ReplyId.empty())))
	{
		std::vector<std::shared_ptr<ForumPostNode>>* replyVec = findReply(OpId, ReplyId).second;
		int replyId = ReplyId.back();
		// Erase reply node smart pointer within the vector it is contained in, thus deleting the reply node and all replies to it
		replyVec->erase(replyVec->begin() + replyId);
	}
}

void WebayForumData::modifyReplyPost(const int& OpId, std::vector<int> ReplyId, const std::string& content)
{
	if ((forumOpList[OpId] != nullptr) || (!forumOpList[OpId]->replies.empty()))
		findReply(OpId, ReplyId).first->postContent = content;
} // end modifyReplyPost

void WebayForumData::modifyOpPost(const int& OpId, const std::string& content)
{
	if (forumOpList[OpId] != nullptr)
		forumOpList[OpId]->postContent = content;
} // end modifyOpPost

std::string WebayForumData::searchPostTitle(const std::string& input)
{
	std::vector<int> matchIds = returnMatchOpIds(input);
	std::string returnString = returnOpPostList(matchIds);
	return returnString;
}

// Search for a specific username for replies that are directly to a post
std::string WebayForumData::searchUserDirectReplies(const int& OpId, const std::string& input, const char& sortBy = 'o')
{
	std::string fullPost = "";
	if (!forumOpList[OpId]->replies.empty())
	{
		std::vector<int> matchIds = returnMatchDirectReplyIds(OpId, input);

		std::string opPostId = std::to_string(OpId);
		std::string opUser = forumOpList[OpId]->postUser;
		std::string opTitle = forumOpList[OpId]->postTitle;
		std::string opDate = forumOpList[OpId]->postDate;
		std::string opTime = forumOpList[OpId]->postTime;
		std::string opContent = forumOpList[OpId]->postContent;
		std::string replyStrings = "";

		if (!forumOpList[OpId]->replies.empty()) {
			unsigned int i;
			if (sortBy == 'o')
			{
				for (i = 0; i < matchIds.size(); i++)
					replyStrings += recurReplies(matchIds[i], forumOpList[OpId]->replies[matchIds[i]]);
			}
			else if (sortBy == 'n')
			{
				for (i = matchIds.size(); i > 0; i--)
					replyStrings += recurReplies(matchIds[i - 1], forumOpList[OpId]->replies[matchIds[i-1]]);
			}
		}
		fullPost = "{\"postid\" : " + opPostId + ", \"user\" : \"" + opUser + "\", \"title\" : \"" + opTitle + "\", \"date\" : \"" + opDate + "\", \"time\" : \"" + opTime + "\", \"content\" : \"" + opContent + "\", \"replies\" : [" + replyStrings + "] }";
	}
	return fullPost;
}

// Return id list of matching titles of op posts based on inputted string
std::vector<int> WebayForumData::returnMatchOpIds(const std::string& input)
{
	// OpId, match count
	std::vector<std::pair<int, int>> matchIds;
	std::vector<int> returnMatchIds;

	unsigned int i;
	int matches;
	std::pair<int, int> idAndCount;
	// Iterate forumOpList for matching titles
	for (i = 0; i < forumOpList.size(); i++)
	{
		if (forumOpList[i] != nullptr)
		{
			matches = matchCount(i, input);
			if (matches > 0)
			{
				idAndCount = std::make_pair(i, matches);
				matchIds.push_back(idAndCount);
			}
		}
	}
	
	// Sort by most matches to least matches
	std::sort(matchIds.begin(), matchIds.end(), sortBySecond);

	// Place sorted ids into returning vector
	returnMatchIds.reserve(matchIds.size());
	for (i = 0; i < matchIds.size(); i++)
		returnMatchIds.push_back(matchIds[i].first);

	return returnMatchIds;
} // end searchPostTitle

// Return id list of reply ids from an op post with matching usernames to the inputted string
std::vector<int> WebayForumData::returnMatchDirectReplyIds(const int& OpId, const std::string& input)
{
	std::vector<int> returnVec;
	std::string inputString, directReplyUser;
	inputString = input;
	upperCase(inputString);
	
	unsigned int i;
	for (i = 0; i < forumOpList[OpId]->replies.size(); i++)
	{
		directReplyUser = forumOpList[OpId]->replies[i]->postUser;
		upperCase(directReplyUser);
			if (inputString == directReplyUser)
				returnVec.push_back(i);
	}

	return returnVec;
}

// Return string of post data in json dictionary format
std::string WebayForumData::returnPostData(const int& OpId, const char& sortBy = 'o')
{
	std::string fullPost;
	std::string opPostId = std::to_string(OpId);
	std::string opUser = forumOpList[OpId]->postUser;
	std::string opTitle = forumOpList[OpId]->postTitle;
	std::string opDate = forumOpList[OpId]->postDate;
	std::string opTime = forumOpList[OpId]->postTime;
	std::string opContent = forumOpList[OpId]->postContent;
	std::string replyStrings = "";

	if (!forumOpList[OpId]->replies.empty()) {
		unsigned int i;
		if (sortBy == 'o')
		{
			for (i = 0; i < forumOpList[OpId]->replies.size(); i++)
				replyStrings += recurReplies(i, forumOpList[OpId]->replies[i]);
		}
		else if(sortBy == 'n')
		{
			for (i = forumOpList[OpId]->replies.size(); i > 0; i--)
				replyStrings += recurReplies((i-1), forumOpList[OpId]->replies[i-1]);
		}
	}
	fullPost = "{\"postid\" : " + opPostId + ", \"user\" : \"" + opUser + "\", \"title\" : \"" + opTitle +  "\", \"date\" : \"" + opDate + "\", \"time\" : \"" + opTime + "\", \"content\" : \"" + opContent + "\", \"replies\" : [" + replyStrings + "] }";

	return fullPost;
} // end returnPostContent

// Returns string of op posts in json dictionary format, to be used to return on searches
std::string WebayForumData::returnOpPostList(const std::vector<int>& opIdVec)
{
	std::string fullList = "{";
	std::string opPostId, opUser, opTitle, opDate, opTime;
	
	unsigned int i;
	for (i = 0; i < opIdVec.size(); i++)
	{
		opTitle = forumOpList[opIdVec[i]]->postTitle;
		opUser = forumOpList[opIdVec[i]]->postUser;
		opDate = forumOpList[opIdVec[i]]->postDate;
		opTime = forumOpList[opIdVec[i]]->postTime;
		opPostId = std::to_string(opIdVec[i]);

		if (i != (opIdVec.size() - 1))
		{
			fullList += "{\"postid\" : " + opPostId + ", \"user\" : \"" + opUser + "\", \"title\" : \"" + opTitle + "\", \"date\" : \"" + opDate + "\", \"time\" : \"" + opTime + "\"}, ";
		}
		else
		{
			fullList += "{\"postid\" : " + opPostId + ", \"user\" : \"" + opUser + "\", \"title\" : \"" + opTitle + "\", \"date\" : \"" + opDate + "\", \"time\" : \"" + opTime + "\"}";
		}
	}
	fullList += "}";
	return fullList;
}

// Returns string of op posts in json dictionary format, sortBy o: oldest to newest, n: newest to oldest
std::string WebayForumData::returnFullOpList(const char& sortBy = 'o')
{
	std::string fullList = "{";
	std::string opPostId, opUser, opTitle, opDate, opTime;

	unsigned int i;
	// Sorted by oldest to newest
	if (sortBy == 'o') {
		for (i = 0; i < forumOpList.size(); i++)
		{
			opPostId = std::to_string(i);
			if (forumOpList[i] != nullptr) {
				opTitle = forumOpList[i]->postTitle;
				opUser = forumOpList[i]->postUser;
				opDate = forumOpList[i]->postDate;
				opTime = forumOpList[i]->postTime;
				if (i != (forumOpList.size() - 1))
					fullList += "{\"postid\" : " + opPostId + ", \"user\" : \"" + opUser + "\", \"title\" : \"" + opTitle + "\", \"date\" : \"" + opDate + "\", \"time\" : \"" + opTime + "\"}, ";
				else
					fullList += "{\"postid\" : " + opPostId + ", \"user\" : \"" + opUser + "\", \"title\" : \"" + opTitle + "\", \"date\" : \"" + opDate + "\", \"time\" : \"" + opTime + "\"}, ";
			}
			else
			{
				if (i != (forumOpList.size() - 1))
					fullList += "{\"postid\" : " + opPostId + ", \"user\" : \"deleted\", \"title\" : \"deleted\" \"date\" : \"deleted\", \"time\" : \"deleted\"}, ";
				else
					fullList += "{\"postid\" : " + opPostId + ", \"user\" : \"deleted\", \"title\" : \"deleted\" \"date\" : \"deleted\", \"time\" : \"deleted\"}";
			}
		}
	}
	// Sorted by newest to oldest
	else if (sortBy == 'n')
	{
		for (i = forumOpList.size(); i > 0; i--)
		{
			opPostId = std::to_string(i-1);
			if (forumOpList[i-1] != nullptr) {
				opTitle = forumOpList[i-1]->postTitle;
				opUser = forumOpList[i-1]->postUser;
				if (i != (forumOpList.size() - 1))
				{
					fullList += "{\"postid\" : " + opPostId + ", \"user\" : \"" + opUser + "\", \"title\" : \"" + opTitle + "\", \"date\" : \"" + opDate + "\", \"time\" : \"" + opTime + "\"}, ";
				}
				else
				{
					fullList += "{\"postid\" : " + opPostId + ", \"user\" : \"" + opUser + "\", \"title\" : \"" + opTitle + "\", \"date\" : \"" + opDate + "\", \"time\" : \"" + opTime + "\"}";
				}
			}
			else
			{
				if (i != (forumOpList.size() - 1))
					fullList += "{\"postid\" : " + opPostId + ", \"user\" : \"deleted\", \"title\" : \"deleted\" \"date\" : \"deleted\", \"time\" : \"deleted\"}, ";
				else
					fullList += "{\"postid\" : " + opPostId + ", \"user\" : \"deleted\", \"title\" : \"deleted\" \"date\" : \"deleted\", \"time\" : \"deleted\"}";
			}
		}
	}
	
	fullList += "}";
	return fullList;
}

// Recurse through undefined amount of replies to compile together into a json dictionary style string
std::string WebayForumData::recurReplies(const int& replyId, std::shared_ptr<ForumPostNode> replyPtr)
{
	std::string replyData;
	std::string replyStrings = "";

	if (!replyPtr->replies.empty())
	{
		unsigned int i;
		for (i = 0; i < replyPtr->replies.size(); i++)
			if(i != (replyPtr->replies.size()-1))
				replyStrings += (recurReplies(i, replyPtr->replies[i]) + ", ");
			else
				replyStrings += (recurReplies(i, replyPtr->replies[i]));
	}

	std::string replyIdString = std::to_string(replyId);
	std::string replyUser = replyPtr->postUser;
	std::string replyContent = replyPtr->postContent;
	std::string replyDate = replyPtr->postDate;
	std::string replyTime = replyPtr->postTime;

	replyData = "{\"replyid\" : " + replyIdString + ", \"user\" : \"" + replyUser + "\", \"content\" : \"" + replyContent + "\", \"date\" : \"" + replyDate + "\", \"time\" : \"" + replyTime + "\", \"replies\" : [" + replyStrings + "] }";
	return replyData;
}

// Reserver space for vector
void WebayForumData::reserveListSpace()
{
	// If empty, initalize space for vector, otherwise, reserve more to prevent reallocation, which would invalidate pointers
	if (forumOpList.empty())
		forumOpList.reserve(50);
	else
		forumOpList.reserve(forumOpList.capacity() + 50);
} // end reserveListSpace

// Clean up deleted vectors that are still on the main forum op list
void WebayForumData::cleanUp()
{
	unsigned int i;
	for (i = 0; i < deletedOpList.size(); i++)
		forumOpList.erase(forumOpList.begin() + deletedOpList[i]);
}

// Test if there is space in memory for more objects
bool WebayForumData::full()
{
	ForumOP* temp;

	temp = new ForumOP;
	if (temp == NULL)
		return true;
	else
		return false;

	delete temp;
} // end full

// Destroy all data when program is finished
void WebayForumData::destroyData()
{
	/* in case resize does not work for whatever reason
	unsigned int i;
	
	for(i = 0 ; i < forumOpList.size(); i++)
		forumOpList.pop_back(); */

	forumOpList.resize(0);
} // end destroyData

//Iterate to find a reply using the reply id, returns a pair, first is the reply node pointer, second is the vector pointer the reply is contained in
replyLocationPair  WebayForumData::findReply(const int& OpId, std::vector<int> ReplyId)
{
		std::vector<std::shared_ptr<ForumPostNode>>* replyList1 = &forumOpList[OpId]->replies;
		std::vector<std::shared_ptr<ForumPostNode>>* replyList2;
		unsigned int i;
		replyLocationPair returnPair;
		// Iterate into each nested vector of the reply nodes to find the location of the reply using the reply id
		for (i = 1; i < ReplyId.size(); i++)
		{
			replyList2 = &(*replyList1)[ReplyId[i-1]]->replies;
			replyList1 = replyList2;
		}

		// returns pair of the reply node pointer that would be located at the last id within ReplyId[], and pointer to the vector its pointer is contained in
		returnPair = make_pair(replyList1->at(ReplyId.back()), replyList1);

		return returnPair;
} // end findReply

// Count the matching words between the search query and the title at the OpId
int WebayForumData::matchCount(const int& OpId, const std::string& input)
{
	int matchCount = 0;
	std::string query = input;
	std::string titleName = forumOpList[OpId]->postTitle;

	std::vector<std::string> queryWords = splitString(query);
	std::vector<std::string> titleWords = splitString(titleName);

	unsigned int i;
	unsigned int j;
	for (i = 0; i < queryWords.size(); i++)
		for (j = 0; j < titleWords.size(); j++)
			if (queryWords[i] == titleWords[j])
				matchCount++;

	return matchCount;
} // end findMatch

// Split the input string into just the words which are placed separately into a vector, inputted string needs to be cleaned up first
std::vector<std::string> WebayForumData::splitString(std::string input)
{
	std::vector<std::string> wordsVec;
	
	//clean up string
	upperCase(input);
	cleanString(input);

	unsigned int i;
	int wordStart;
	int wordLength = 0;
	std::string word;

	if (input.length() != 1) 
	{
		for (i = 0; i < input.length(); i++)
		{
			// Start word at string beginning
			if (i == 0)
				wordStart = 0;
			// Start word if previous character was a space
			else if (input[i - 1] == ' ')
			{
				wordStart = i;
				// If current character is at the end of the string
				if (i == (input.length() - 1))
				{
					wordLength++;
					wordsVec.push_back(input.substr(wordStart, wordLength));
					wordLength = 0;
				}
			}
			// If a word comes after a number without a space between them, separate them
			else if ((input[i] != ' ') && (isNumChar(input[i - 1]) && (!isNumChar(input[i]))))
			{
				wordsVec.push_back(input.substr(wordStart, wordLength));
				wordLength = 0;
				wordStart = i;

				// There is only one letter after the number and it is at the end of the string
				if (i == (input.length() - 1))
				{
					wordLength++;
					wordsVec.push_back(input.substr(wordStart, wordLength));
				}
			}
			// If a word has a number after it without spaces between, separate them into different words
			else if (isNumChar(input[i]) && (!isNumChar(input[i - 1])))
			{
				wordsVec.push_back(input.substr(wordStart, wordLength));
				wordLength = 0;
				wordStart = i;

				// If there is only one number after the word at the end of the string
				if (i == (input.length() - 1))
				{
					wordLength++;
					wordsVec.push_back(input.substr(wordStart, wordLength));
				}
			}
			// If at the end of a word, dictated by a space
			else if (input[i] == ' ')
			{
				wordsVec.push_back(input.substr(wordStart, wordLength));
				wordLength = -1;
			}
			// If at end of a word, dictated by being at the end of the string
			else if (i == (input.length() - 1))
			{
				wordLength++;
				wordsVec.push_back(input.substr(wordStart, wordLength));
			}
			wordLength++;

		}
	}
	else
		wordsVec.push_back(input);

	return wordsVec;
} // end splitString

// Converts each valid blackspace character into uppercase letters
void WebayForumData::upperCase(std::string& input)
{
	unsigned int i;
	for (i = 0; i < input.length(); i++)
	{
		input[i] = toupper(input[i]);
	}
} // end upperCase

// Boolean check to determine which characters qualify as valid input to use, including spaces, letters, and numbers
bool WebayForumData::validChar(const char& input)
{
	return (input == ' ' || (input >= 'A' && input <= 'Z') || (input >= '0' && input <= '9'));

} // end validChar

// Cleans each string by removing characters that are not considered valid input
std::string WebayForumData::cleanString(const std::string& input)
{
	std::string result = "";

	unsigned int i;
	for (i = 0; i < input.length(); i++)
	{
		// remove invalid characters and spaces at the front and end of the string, else remove invalid character and extra spaces
		if ((i == 0) || (i == (input.length()-1)))
			if (validChar(input[i]) || (input[i] != ' '))
				result = result + input[i];
		else
			if (validChar(input[i]) || ((input[i-1] != ' ') && (input[i] != ' ')))
				result = result + input[i];
		
	}

	return result;

} // end cleanString

// Check if character is a number
bool WebayForumData::isNumChar(const char& input)
{
	return (input >= '0') && (input <= '9');
}

// Function for sorting found matches from searches in descending order of total matching words
bool WebayForumData::sortBySecond(const std::pair<int, int>& firstPair, const std::pair<int, int>& secondPair)
{
	return (firstPair.second > secondPair.second);
}