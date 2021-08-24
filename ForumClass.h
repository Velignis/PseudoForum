#pragma once

#ifndef WEBAYFORUM_H_
#define WEBAYFORUM_H_

#include <string>
#include <vector>
#include <memory>

struct ForumPostNode;

typedef std::pair<std::shared_ptr<ForumPostNode>, std::vector<std::shared_ptr<ForumPostNode>>*> replyLocationPair;
//both typedef and using cause new to create an overload error upon constructing new objects

// shared pointers are used instead of unique pointers to be able to return them withfindReply()
struct ForumPostNode
{
	std::string postUser;
	std::string postContent;
	std::vector<std::shared_ptr<ForumPostNode>> replies;
	std::string postDate;
	std::string postTime;
};

struct ForumOP : public ForumPostNode
{
	std::string postTitle;
};

class WebayForumData
{
	// Perhaps change all functions that check if memory is full to from void to bool or int to return if it was successful
	public:
		// User name, title, content, date, time
		void addOpPost(const std::string&, const std::string&, const std::string&, const std::string&, const std::string&);
		// OpId, ReplyId, user name, content, date, time
		void addReplyPost(const int&, std::vector<int>, const std::string&, const std::string&, const std::string&, const std::string&);
		// OpId
		void deleteOpPost(const int&);
		// OpId, ReplyId
		void deleteReplyPost(const int&, std::vector<int>);

		//OpId, content
		void modifyOpPost(const int&, const std::string&);
		//OpId, ReplyId, content
		void modifyReplyPost(const int&, std::vector<int>, const std::string&);

		std::string searchPostTitle(const std::string&);
		std::string searchUserDirectReplies(const int&, const std::string&, const char&);
		std::vector<int> returnMatchOpIds(const std::string&);
		std::vector<int> returnMatchDirectReplyIds(const int&, const std::string&);
		std::string returnOpPostList(const std::vector<int>&);
		std::string returnFullOpList(const char&);
		std::string returnPostData(const int&, const char&);

		void reserveListSpace();
		void cleanUp();
		void destroyData();

		replyLocationPair findReply(const int&, std::vector<int>);
		


	private:
		std::vector<std::unique_ptr<ForumOP>> forumOpList;
		std::vector<int> deletedOpList;
		bool full();

		std::string recurReplies(const int&, std::shared_ptr<ForumPostNode>);

		int matchCount(const int&, const std::string&);
		std::vector<std::string> splitString(std::string);
		void upperCase(std::string&);
		bool validChar(const char&);
		std::string cleanString(const std::string&);
		bool isNumChar(const char& input);
		static bool sortBySecond(const std::pair<int, int>&, const std::pair<int, int>&);
};


#endif /*WEBAYFORUM_H_*/