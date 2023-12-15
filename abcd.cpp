#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

// Behavior Tree Node Interface
class BehaviorNode {
public:
    virtual ~BehaviorNode() {}
    virtual bool execute() = 0;
};

// Action Node
class ActionNode : public BehaviorNode {
public:
    ActionNode(const std::string& actionName) : actionName(actionName) {}

    bool execute() override {
        std::cout << "Executing Action: " << actionName << std::endl;
        return true;  // In a real scenario, this could return success/failure based on the action.
    }

private:
    std::string actionName;
};

// Condition Node
class ConditionNode : public BehaviorNode {
public:
    ConditionNode(const std::string& conditionName, bool condition) : conditionName(conditionName), condition(condition) {}

    bool execute() override {
        std::cout << "Checking Condition: " << conditionName << std::endl;
        return condition;
    }

private:
    std::string conditionName;
    bool condition;
};

// Sequence Node
class SequenceNode : public BehaviorNode {
public:
    SequenceNode(const std::string& sequenceName) : sequenceName(sequenceName) {}

    void addChild(BehaviorNode* child) {
        children.push_back(child);
    }

    bool execute() override {
        std::cout << "Executing Sequence: " << sequenceName << std::endl;
        for (BehaviorNode* child : children) {
            if (!child->execute()) {
                return false;
            }
        }
        return true;
    }

private:
    std::string sequenceName;
    std::vector<BehaviorNode*> children;
};

// Selector Node
class SelectorNode : public BehaviorNode {
public:
    SelectorNode(const std::string& selectorName) : selectorName(selectorName) {}

    void addChild(BehaviorNode* child) {
        children.push_back(child);
    }

    bool execute() override {
        std::cout << "Executing Selector: " << selectorName << std::endl;
        for (BehaviorNode* child : children) {
            if (child->execute()) {
                return true;
            }
        }
        return false;
    }

private:
    std::string selectorName;
    std::vector<BehaviorNode*> children;
};

int main() {
    // Seed for randomization
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Define Action Nodes
    ActionNode* moveToWaypoint = new ActionNode("Move to Waypoint");
    ActionNode* attackEnemy = new ActionNode("Attack Enemy");
    ActionNode* idle = new ActionNode("Idle");

    // Define Condition Nodes
    ConditionNode* isEnemyVisible = new ConditionNode("Is Enemy Visible?", (std::rand() % 2 == 0));

    // Define Sequence and Selector Nodes
    SequenceNode* patrolSequence = new SequenceNode("Patrol Sequence");
    patrolSequence->addChild(moveToWaypoint);
    patrolSequence->addChild(new SelectorNode("Attack or Idle Selector"));
    patrolSequence->addChild(isEnemyVisible);
    patrolSequence->addChild(attackEnemy);

    SelectorNode* rootSelector = new SelectorNode("Root Selector");
    rootSelector->addChild(patrolSequence);
    rootSelector->addChild(idle);

    // Execute the Behavior Tree
    std::cout << "=== Behavior Tree Execution ===" << std::endl;
    rootSelector->execute();

    // Cleanup
    delete rootSelector;  // This will recursively delete all nodes

    return 0;
}
