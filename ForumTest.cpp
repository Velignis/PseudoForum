#include <string>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include "WebayForum.h"

std::vector<int> replyId(std::string);
bool isNumChar(const char&);
const std::string currentDate();
const std::string currentTime();

int main()
{
    WebayForumData forum;
    int option, postId;
    unsigned int i = 0;
    std::string searchQuery, userName, postTitle, postContent, replyString, idString, optionString, sortBy;//, postDate, postTime;
    std::vector<int> idVector;

    do
    {
        std::cout << "Welcome to the forum" << std::endl << std::endl;
        std::cout << "Enter what you would like to do:" << std::endl;
        std::cout << "0: see the full list of posts" << std::endl;
        std::cout << "1: search for an existing post" << std::endl;
        std::cout << "2: add new post" << std::endl;
        std::cout << "3: delete existing post" << std::endl;
        std::cout << "4: view the contents of a post" << std::endl;
        std::cout << "5: modify existing post" << std::endl;
        std::cout << "6: reply to a post or reply" << std::endl;
        std::cout << "7: delete a reply" << std::endl;
        std::cout << "8: modify existing reply" << std::endl;
        std::cout << "9: search from direct reply from user" << std::endl;
        std::cout << "10: exit forum" << std::endl;
        
        std::getline(std::cin, optionString);
        if (isNumChar(optionString[0]))
            option = std::stoi(optionString);
        else if (isNumChar(optionString[0]) && isNumChar(optionString[1]))
            option = std::stoi(optionString);
        else
            option = 11;

        std::cout << std::endl;

        switch (option)
        {
        case 0:
            std::cout << "Sort from oldest to newest (Enter: o2n), or from newest to oldest (Enter: n2o)" << std::endl;
            std::getline(std::cin, sortBy);
            std::cout << std::endl;

            if(sortBy == "o2n")
                std::cout << forum.returnFullOpList('o') << std::endl << std::endl;
            else if(sortBy == "n2o")
                std::cout << forum.returnFullOpList('n') << std::endl << std::endl;
            break;
        case 1:
            std::cout << "Enter Search Query: ";
            std::getline(std::cin, searchQuery);
            std::cout << std::endl;
            std::cout << forum.searchPostTitle(searchQuery);
            break;
        case 2:
            std::cout << "Enter user name: ";
            std::getline(std::cin, userName);
            
            std::cout << std::endl;
            std::cout << "Enter post title: ";
            std::getline(std::cin, postTitle);

            std::cout << std::endl;
            std::cout << "Enter content of the post: ";
            std::getline(std::cin, postContent);
            /*
            std::cout << std::endl;
            std::cout << "Enter date of the post (MM/DD/YYYY): ";
            std::getline(std::cin, postDate);

            std::cout << std::endl;
            std::cout << "Enter time of the post (24HR)(HH:MM): ";
            std::getline(std::cin, postTime);
            */
            std::cout << std::endl;
            forum.addOpPost(userName, postTitle, postContent, currentDate(), currentTime());
            std::cout << std::endl;
            
            std::cout << "Post added"<< std::endl;

            break;
        case 3:
            std::cout << "Enter post id to delete: ";
            std::getline(std::cin, idString);
            postId = std::stoi(idString);
            std::cout << std::endl;

            forum.deleteOpPost(postId);

            std::cout << "Post deleted" << std::endl;

            break;
        case 4:
            std::cout << "Enter post id to view: ";
            std::getline(std::cin, idString);
            postId = std::stoi(idString);
            std::cout << std::endl;
            std::cout << "Sort direct replies to post from oldest to newest (Enter: o2n), or from newest to oldest (Enter: n2o)" << std::endl;
            std::getline(std::cin, sortBy);
            std::cout << std::endl;

            if (sortBy == "o2n")
                std::cout << forum.returnPostData(postId, 'o');
            else if(sortBy == "n2o")
                std::cout << forum.returnPostData(postId, 'n');
            std::cout << std::endl;

            break;
        case 5:
            std::cout << "Enter post id to modify content of: ";
            std::getline(std::cin, idString);
            postId = std::stoi(idString);
            std::cout << std::endl;
            std::cout << "Enter new content: ";
            std::getline(std::cin, postContent);
            std::cout << std::endl;

            forum.modifyOpPost(postId, postContent);

            std::cout << "Content modified" << std::endl;

            break;
        case 6:
            std::cout << "Enter post id to reply at: ";
            std::getline(std::cin, idString);
            postId = std::stoi(idString);

            std::cout << std::endl;
            std::cout << "Enter reply id, else, enter a nonnumber to reply to post(Enter list of numbers of any amount, example: 1, 2, 3): ";
            std::getline(std::cin, replyString);

            std::cout << std::endl;
            std::cout << "Enter username: ";
            std::getline(std::cin, userName);

            std::cout << std::endl;
            std::cout << "Enter reply content: ";
            std::getline(std::cin, postContent);
            /*
            std::cout << std::endl;
            std::cout << "Enter date of the post (MM/DD/YYYY): ";
            std::getline(std::cin, postDate);

            std::cout << std::endl;
            std::cout << "Enter time of the post (HH:MM): ";
            std::getline(std::cin, postTime);
            std::cout << std::endl;
            */
            idVector = replyId(replyString);
            forum.addReplyPost(postId, idVector, userName, postContent, currentDate(), currentTime());

            std::cout << "Reply added" << std::endl;

            break;
        case 7:
            std::cout << "Enter post id the reply will be delete from: ";
            std::getline(std::cin, idString);
            postId = std::stoi(idString);
            std::cout << std::endl;
            std::cout << "Enter reply id (Enter list of numbers of any amount, example: 1, 2, 3): ";
            std::getline(std::cin, replyString);
            std::cout << std::endl;

            idVector = replyId(replyString);
            for (i = 0; i < idVector.size(); i++)
                std::cout << idVector[i];
            std::cout << "vector";
            forum.deleteReplyPost(postId, idVector);

            std::cout << "Reply delete" << std::endl;

            break;
        case 8:
            std::cout << "Enter post id the reply modified from: ";
            std::getline(std::cin, idString);
            postId = std::stoi(idString);
            std::cout << std::endl;
            std::cout << "Enter reply id (Enter list of numbers of any amount, example: 1, 2, 3): ";
            std::getline(std::cin, replyString);
            std::cout << std::endl;
            std::cout << "Enter new content: ";
            std::getline(std::cin, postContent);
            std::cout << std::endl;

            idVector = replyId(replyString);
            forum.modifyReplyPost(postId, idVector, postContent);

            std::cout << "Reply content modified" << std::endl;

            break;
        case 9:
            std::cout << "Enter post id you want to search in: ";
            std::getline(std::cin, idString);
            postId = std::stoi(idString);
            std::cout << std::endl;
            std::cout << "Enter user name you wish to search for: ";
            std::getline(std::cin, userName);
            std::cout << std::endl;
            std::cout << "Sort direct replies found from oldest to newest (Enter: o2n), or from newest to oldest (Enter: n2o)" << std::endl;
            std::getline(std::cin, sortBy);
            std::cout << std::endl;

            if (sortBy == "o2n")
                std::cout << forum.searchUserDirectReplies(postId, userName, 'o');
            else if (sortBy == "n2o")
                std::cout << forum.searchUserDirectReplies(postId, userName, 'n');
            std::cout << std::endl;
            
            break;
        default:
            std::cout << "Invalid input" << std::endl;
        }
        std::cout << std::endl;
    } while (option != 10);

    forum.destroyData();

    return 0;
}

std::vector<int> replyId(std::string input)
{
    std::vector<int> numVec;

    int numStart, numLength,  id;
    numLength = 0;
    numStart = 0;

    std::string idString;

    unsigned int i;
    for (i = 0; i < input.length(); i++)
    {
        if (isNumChar(input[i])) {
            if (numLength == 0)
                numStart = i;
            numLength++;
            if ((i == (input.length() - 1)))
            {
                idString = input.substr(numStart, numLength);
                id = std::stoi(idString);
                numVec.push_back(id);
                numLength = 0;
                std::cout << id;
            }
        }
        else
        {
            idString = input.substr(numStart, numLength);
            if (!idString.empty()) {
                std::cout << idString << std::endl;
                id = std::stoi(idString);
                numVec.push_back(id);
            }
            numLength = 0;
        }
    }

    return numVec;
}

bool isNumChar(const char& input)
{
    return (input >= '0') && (input <= '9');
}

const std::string currentDate() {
    struct tm timeStruct;
    time_t now = time(0);
    localtime_s(&timeStruct, &now);
    std::string returnDate = (std::to_string(timeStruct.tm_mon+1) + '/'+ std::to_string(timeStruct.tm_mday) + '/' + std::to_string(timeStruct.tm_year-100+2000));

    return returnDate;
}

const std::string currentTime() {
    struct tm timeStruct;
    time_t now = time(0);
    localtime_s(&timeStruct, &now);
    std::string minute, hour;

    if (timeStruct.tm_min < 10)
        hour = ("0" + std::to_string(timeStruct.tm_hour));
    else
        hour = std::to_string(timeStruct.tm_hour);

    if (timeStruct.tm_min < 10)
        minute = ("0" + std::to_string(timeStruct.tm_min));
    else
        minute = std::to_string(timeStruct.tm_min);

    std::string returnTime = (std::to_string(timeStruct.tm_hour) + ":" + minute);

    return returnTime;
}