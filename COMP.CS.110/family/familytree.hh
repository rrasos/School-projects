/*
#############################################################################
# COMP.CS.110 Ohjelmointi 2: Tekniikat / Programming 2: Techniques          #
# Project: Suku on pahin / All in the family                                #
# File: familytree.hh                                                       #
# Description: Familytree class                                             #
#   Contains a datastructure that is populated with Person structs and      #
#   provides some query functions.                                          #
# Notes: * This is a part of an exercise program                            #
#        * Student's aren't allowed to alter public interface!              #
#        * All changes to private side are allowed.                         #
#############################################################################
*/
#ifndef FAMILYTREE_HH
#define FAMILYTREE_HH

#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <iostream>



using Params = const std::vector<std::string>&;

// Named constants to improve readability in other modules.
const std::string NO_ID = "";
const int NO_HEIGHT = -1;

// Error messages
const std::string ALREADY_ADDED = "Error. Person already added.";
const std::string WRONG_LEVEL = "Error. Level can't be less than 1.";

// Struct for the persons data.
struct Person
{
    std::string id_ = NO_ID;
    int height_ = NO_HEIGHT;
    std::vector<Person*> parents_{nullptr, nullptr};
    std::vector<Person*> children_;
};

using IdSet = std::set<std::string>;

/**
 * @brief The Familytree class
 */
class Familytree
{
public:
    /**
     * @brief Familytree
     */
    Familytree();

    /**
     * @brief addNewPerson
     * @param id
     * @param height
     * @param output
     * Add a new person to the datastructure.
     */
    void addNewPerson(const std::string& id, int height,
                      std::ostream &output);

    /**
     * @brief addRelation
     * @param child
     * @param parents
     * @param output
     * Add a new parent-child relation.
     */
    void addRelation(const std::string& child,
                     const std::vector<std::string>& parents,
                     std::ostream& output);

    /**
     * @brief printPersons
     * @param output
     * Print all stored persons with their ids and heights.
     */
    void printPersons(Params, std::ostream &output) const;

    /**
     * @brief printChildren
     * @param params (contains person's id)
     * @param output
     * Print all children of the given person.
     */
    void printChildren(Params params, std::ostream& output) const;

    /**
     * @brief printParents
     * @param params (contains person's id)
     * @param output
     * Print all parents (one or two) of the given person.
     */
    void printParents(Params params, std::ostream& output) const;

    /**
     * @brief printSiblings
     * @param params (contains person's id)
     * @param output
     * Print all siblings of the given person.
     */
    void printSiblings(Params params, std::ostream& output) const;

    /**
     * @brief printCousins
     * @param params (contains person's id)
     * @param output
     * Print all cousins of the given person.
     */
    void printCousins(Params params, std::ostream& output) const;

    /**
     * @brief printTallestInLineage
     * @param params (contains person's id)
     * @param output
     * Print the tallest person of the given person's lineage.
     */
    void printTallestInLineage(Params params, std::ostream& output) const;

    /**
     * @brief printShortestInLineage
     * @param params (contains person's id)
     * @param output
     * Print the shortest person of the given person's lineage.
     */
    void printShortestInLineage(Params params, std::ostream& output) const;

    /**
     * @brief printGrandChildrenN
     * @param params (contains person's id, and distance as a string)
     * @param output
     * Print all grandchildren in the given distance of the given person.
     */
    void printGrandChildrenN(Params params, std::ostream& output) const;

    /**
     * @brief printGrandParentsN
     * @param params
     * @param output
     * Print all grandparents in the given distance of the given person.
     */
    void printGrandParentsN(Params params, std::ostream& output) const;

private:
    /* The following functions are meant to make project easier.
     * You can implement them if you want and/or create your own.
     * Anyway it would be a good idea to implement more functions
     * to make things easier and to avoid "copy-paste-coding".
     */

    //container for all people key=id (for alphabetic prints)
    std::map<std::string, std::shared_ptr<Person>> people_;

    /**
     * @brief getPointer
     * @param id
     * @return the pointer to Person object with the given id.
     */
    std::shared_ptr<Person> getPointer(const std::string& id) const;

    /**
     * @brief printNotFound
     * @param id
     * @param output
     * Print the error message if id not found.
     */
    void printNotFound(const std::string& id, std::ostream& output) const;

    /**
     * @brief personExists
     * @param id
     * @return true/false
     * checks if person exist and calls printNotFound if nessesary
     */
    bool personExists(const std::string& id, std::ostream& output) const;


    /**
     * @brief vectorToIdSet
     * @param container
     * @return set of ids of those persons included in the given container
     * vector.
     */
    IdSet vectorToIdSet(const std::vector<Person*> &container,
                        const std::string &id = "") const;

    /**
     * @brief printGroup
     * @param id
     * @param group can be e.g. "children", "parents", "siblings", "cousins"^
     * @param container contains the ids of the group members
     * @param output
     * Print the data in the container.
     */
    void printGroup(const std::string& id, const std::string& group,
                    const IdSet& container, std::ostream& output) const;

    /**
     * @brief findMinMaxInLineage
     * @param person
     * @param type type can be e.g "parents" or "childs"
     * @return Person* person
     * finds tallest or shortes in persons lineage
     */
    Person* findMinMaxInLineage(Person* person, std::string type) const;


    /**
     * @brief findgrand
     * @param person
     * @param n
     * @param type can be e.g "parents" or "childs"
     * @return all found grand parents/childrens
     */
    std::vector<Person*> findGrand(Person* person,
                                   int n, std::string type,
                                   std::vector<Person*> grand = {}) const;


    /**
     * @brief multiplyer
     * @param n
     * @param type can be e.g parents or childs
     * @return  [(n-1)*great-]type
     */
    std::string multiplyer (int n, std::string type) const;

};

#endif // FAMILYTREE_HH
