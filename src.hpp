#include<iostream>
#include<cstdio>
#include<cstring>
#include<cmath>
#include<string>

class BaseJudger {
public:
    BaseJudger (size_t time_limit , size_t memory_limit ,
                const char *answer) : time_limit_(time_limit), memory_limit_(memory_limit), score_(0) {
        // Allocate and copy the answer string
        answer_ = new char[strlen(answer) + 1];
        strcpy(answer_, answer);
    }

    virtual void Submit (size_t time , size_t memory , const char *output) = 0;

    size_t GetScore () const { return score_; }

    virtual ~BaseJudger () {
        // Clean up dynamically allocated memory
        delete[] answer_;
    };

protected:
    char *answer_;
    const size_t time_limit_;
    const size_t memory_limit_;
    size_t score_;

    virtual bool CheckAnswer (const char *output) const {
        // the output must equal to the answer
        return strcmp(answer_ , output) == 0;
    }
};


class ICPCJudger : public BaseJudger {
public:
    ICPCJudger(size_t time_limit, size_t memory_limit, const char *answer)
        : BaseJudger(time_limit, memory_limit, answer) {}

    void Submit(size_t time, size_t memory, const char *output) override {
        // Check if time and memory are within limits and answer is correct
        if (time <= time_limit_ && memory <= memory_limit_ && CheckAnswer(output)) {
            // Take maximum score (can submit multiple times)
            if (score_ < 100) {
                score_ = 100;
            }
        }
        // If not passed, score remains unchanged (could be 0 or previous max)
    }
};


class OIJudger : public BaseJudger {
private:
    bool has_submitted_;

    // Helper function to remove trailing spaces from a string
    std::string removeTrailingSpaces(const std::string& str) const {
        std::string result;
        size_t pos = 0;
        size_t len = str.length();
        
        while (pos < len) {
            size_t line_end = str.find('\n', pos);
            if (line_end == std::string::npos) {
                line_end = len;
            }
            
            // Get the line
            std::string line = str.substr(pos, line_end - pos);
            
            // Remove trailing spaces from this line
            size_t last_non_space = line.find_last_not_of(' ');
            if (last_non_space != std::string::npos) {
                result += line.substr(0, last_non_space + 1);
            }
            // If the line is all spaces, add empty string (nothing)
            
            // Add newline if this wasn't the last line
            if (line_end < len) {
                result += '\n';
            }
            
            pos = line_end + 1;
        }
        
        return result;
    }

protected:
    bool CheckAnswer(const char *output) const override {
        // OIJudger ignores trailing spaces on each line
        std::string cleaned_answer = removeTrailingSpaces(answer_);
        std::string cleaned_output = removeTrailingSpaces(output);
        return cleaned_answer == cleaned_output;
    }

public:
    OIJudger(size_t time_limit, size_t memory_limit, const char *answer)
        : BaseJudger(time_limit, memory_limit, answer), has_submitted_(false) {}

    void Submit(size_t time, size_t memory, const char *output) override {
        if (has_submitted_) {
            // Multiple submissions result in zero score
            score_ = 0;
            return;
        }
        
        has_submitted_ = true;
        
        // Check if time and memory are within limits and answer is correct
        if (time <= time_limit_ && memory <= memory_limit_ && CheckAnswer(output)) {
            score_ = 100;
        } else {
            score_ = 0;
        }
    }
};


class SpacialJudger : public BaseJudger {
private:
    size_t full_score_time_;
    size_t full_score_memory_;

public:
    SpacialJudger(size_t time_limit, size_t memory_limit, 
                  size_t full_score_time, size_t full_score_memory, 
                  const char *answer)
        : BaseJudger(time_limit, memory_limit, answer),
          full_score_time_(full_score_time),
          full_score_memory_(full_score_memory) {}

    void Submit(size_t time, size_t memory, const char *output) override {
        // First check if answer is correct
        if (!CheckAnswer(output)) {
            // Answer is wrong, score remains unchanged (could be 0 or previous max)
            return;
        }
        
        // Calculate time_score
        size_t time_score;
        if (time <= full_score_time_) {
            time_score = 100;
        } else if (time >= time_limit_) {
            time_score = 0;
        } else {
            // Linear interpolation between full_score_time and time_limit
            // time_score = 100 - (time - full_score_time) * 100 / (time_limit - full_score_time)
            time_score = (size_t)floor(100.0 * (time_limit_ - time) / (time_limit_ - full_score_time_));
        }
        
        // Calculate memory_score
        size_t memory_score;
        if (memory <= full_score_memory_) {
            memory_score = 100;
        } else if (memory >= memory_limit_) {
            memory_score = 0;
        } else {
            // Linear interpolation between full_score_memory and memory_limit
            memory_score = (size_t)floor(100.0 * (memory_limit_ - memory) / (memory_limit_ - full_score_memory_));
        }
        
        // Calculate final score
        size_t current_score = (size_t)floor(time_score * memory_score / 100.0);
        
        // Take maximum score (can submit multiple times)
        if (current_score > score_) {
            score_ = current_score;
        }
    }
};
