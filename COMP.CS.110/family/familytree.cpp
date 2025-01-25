/*
#############################################################################
# COMP.CS.110 Ohjelmointi 2: Tekniikat / Programming 2: Techniques          #
# Project: Suku on pahin / All in the family                                #
# File: familytree.cpp                                                      #
# Description: Familytree class                                             #
# Contains functions for familytree class                                   #
# Rasmus sorila                                                             #
# student number 150267845                                                  #
# rrraso                                                                    #
# rasmus.sorila@tuni.fi                                                     #
#############################################################################
*/
#include "familytree.hh"

using Ptr = const std::shared_ptr<Person>;



Familytree::Familytree()
{

}

void Familytree::addNewPerson(const std::string &id,
                              int height, std::ostream &output)
{
    //checks if person already exist
    if (getPointer(id) != nullptr){
        output << ALREADY_ADDED << std::endl;
        return;
    }
    //create new_person using shared pointer
    Ptr new_person = std::make_shared<Person>
            (Person{id, height,{nullptr},{nullptr}});

    //adds the new person to the people_ map
    people_[id]=new_person;

}

void Familytree::addRelation(const std::string &child,
                             const std::vector<std::string> &parents,
                             std::ostream &output)
{
    if (personExists(child,output)){
        Ptr child_ptr=getPointer(child);
        for (const auto &parent : parents){
            if (!parent.empty()){
                Ptr parent_ptr =getPointer(parent);

                //parent ptr is not nullptr
                if (parent_ptr){
                    child_ptr->parents_.push_back(parent_ptr.get());

                    //links child to parents children's
                    parent_ptr->children_.push_back(child_ptr.get());
                }
            }
        }
    }
}

void Familytree::printPersons(Params, std::ostream &output) const
{
    for (const auto& pair : people_){
        output << pair.second->id_;
        output << ", ";
        output << pair.second->height_ << std::endl;
    }
}

void Familytree::printChildren(Params params, std::ostream &output) const
{
    std::string id = params.at(0);
    if (personExists(id,output)){
        std::string group= "children";
        IdSet cont = vectorToIdSet(getPointer(id)->children_);
        printGroup(id, group, cont, output);
    }
}

void Familytree::printParents(Params params, std::ostream &output) const
{
    std::string id = params.at(0);
    if (personExists(id,output)){
        std::string group= "parents";
        IdSet cont = vectorToIdSet(getPointer(id)->parents_);
        printGroup(id, group, cont, output);
    }
}

void Familytree::printSiblings(Params params, std::ostream &output) const
{
    std::string id = params.at(0);
    if (personExists(id,output)){
        std::string group= "siblings";
        Ptr person = getPointer(id);
        IdSet cont;
        Person* parent = person->parents_.back();

        if (parent){
            cont = vectorToIdSet(parent->children_,id);
        }
        printGroup(id, group, cont, output);
    }
}

void Familytree::printCousins(Params params, std::ostream &output) const
{
    std::string id = params.at(0);
    if (personExists(id,output)){
        std::string group = "cousins";
        Ptr person = getPointer(id);
        std::vector<Person*> cousings;

        //gets the parent
        for (const auto& parent : person->parents_){
            if (parent){

                //gets  one grandparent
                Person* gp = parent->parents_.back();

                if (gp != nullptr){

                //gets uncles or aunts (doesnt indclude own parents)
                    for (const auto& parent_s :
                            vectorToIdSet(gp->children_,parent->id_)){

                         //gets cousings and adds them to the vec
                         for (const auto& cos :
                                    people_.find(parent_s)->second->children_){
                            if (cos){
                                    cousings.push_back(cos);
                            }
                        }
                    }
                }
            }
        }
    IdSet cont = vectorToIdSet(cousings);
    printGroup(id, group, cont, output);
    }
}

void Familytree::printTallestInLineage(Params params,
                                       std::ostream &output) const
{
    std::string id = params.at(0);
    if (personExists(id,output)){
        Ptr person = getPointer(id);

        //get the tallest person in lineage
        Person* tallest =findMinMaxInLineage(person.get(), "t");

        if (tallest){
            output << "With the height of ";
            output << tallest->height_;
            output << ", " << tallest->id_;
            output << " is the tallest person in ";


            //if person is the tallest
            if (tallest == person.get()){
                output << "his/her lineage." << std::endl;
            }

            //if somebody else is tallest
            else {
                output << person->id_;
                output << "'s lineage." << std::endl;
            }
        }
    }
}

void Familytree::printShortestInLineage(Params params,
                                        std::ostream &output) const
{
    std::string id = params.at(0);
    if (personExists(id,output)){
        Ptr person = getPointer(id);

        //get the shortest person in lineage
        Person* shortest = findMinMaxInLineage(person.get(),"s");

        if (shortest){
            output << "With the height of ";
            output << shortest->height_;
            output << ", " << shortest->id_;
            output << " is the shortest person in ";

            //person is the sortes
            if (shortest == person.get()){
                output << "his/her lineage." << std::endl;
            }

            //if somebody else is shortest
            else {
                output << person->id_;
                output << "'s lineage." << std::endl;
            }
        }
    }
}

void Familytree::printGrandChildrenN(Params params,
                                     std::ostream &output) const
{
    std::string type = "grandchildren";
    int n = stoi(params.at(1));

    //check input
    if (n < 1) {
        output << "Error. Level can't be less than 1." << std::endl;
        return;
    }

    std::string id = params.at(0);
    if (personExists(id, output)) {
        Ptr person = getPointer(id);
        std::vector<Person*> vec = findGrand(person.get(), n, type);
        IdSet cont = vectorToIdSet(vec);
        std::string group = multiplyer(n,type);
        printGroup(id, group, cont, output);

    }
}

void Familytree::printGrandParentsN(Params params,
                                    std::ostream &output) const
{
    std::string type = "grandparents";
    int n = stoi(params.at(1));

    //check input
    if (n < 1) {
        output << "Error. Level can't be less than 1." << std::endl;
        return;
    }

    std::string id = params.at(0);
    if (personExists(id, output)) {
        Ptr person = getPointer(id);
        std::vector<Person*> vec = findGrand(person.get(), n, type);
        IdSet cont = vectorToIdSet(vec);
        std::string group = multiplyer(n,type);
        printGroup(id, group, cont, output);

    }
}

std::shared_ptr<Person> Familytree::getPointer(const std::string &id) const
{
    auto it = people_.find(id);
    if (it != people_.end()){
        return it->second;
    }
    return nullptr;
}

void Familytree::printNotFound(const std::string &id,
                               std::ostream &output) const
{
        output << "Error. ";
        output << id;
        output << " not found." << std::endl;

}

bool Familytree::personExists(const std::string &id,
                              std::ostream &output) const
{
    //check that person exist and prints error if not
    if (!getPointer(id)){
        printNotFound(id,output);
        return false;
    }
    return true;
}

IdSet Familytree::vectorToIdSet(const std::vector<Person *> &container,
                                const std::string &id) const
{
    IdSet result;
    for (const auto& person : container){
        if (person != nullptr && person->id_!=id){
            result.insert(person->id_);
        }
    }
    return result;
}

void Familytree::printGroup(const std::string &id,
                            const std::string &group,
                            const IdSet &container,
                            std::ostream &output) const
{
    output << id;
    output << " has ";

    //if nobody in group
    if (container.empty()){
        output << "no " << group << "." << std::endl;
    }
    else {
        output << container.size() << " ";
        output << group << ":" << std::endl;
        for (auto &i : container){
            output << i << std::endl;
        }
    }
}

Person *Familytree::findMinMaxInLineage(Person *person,
                                        std::string type) const
{
    if (!person){
        return nullptr;
    }
    Person* crit_person = person;
    for(const auto& child : person->children_){
        Person* crit_in_lin = findMinMaxInLineage(child,type);

        //tallest in lineage
        if (type == "t"){
            if (crit_in_lin &&
                    crit_in_lin->height_ > crit_person->height_){
                crit_person = crit_in_lin;
            }
        }

        //shortes in lineage
        if (type == "s"){
            if (crit_in_lin &&
                    crit_in_lin->height_ < crit_person->height_){
                crit_person = crit_in_lin;
            }
        }
    }
    return crit_person;
}

std::vector<Person *> Familytree::findGrand(Person *person,
                                            int n, std::string type,
                                            std::vector<Person*> grand) const
{
    if (!person) {
        return grand;
    }

    if (type == "grandchildren") {
        if (n == 0) {
            for (const auto& child : person->children_) {
                if (child) {
                    grand.push_back(child);
                }
            }
            return grand;
        }

        for (const auto& child : person->children_) {
            if (child) {
                grand = findGrand(child, n-1, type, grand);
            }
        }
    }

    if (type == "grandparents") {
        if (n == 0) {
            for (const auto& parent : person->parents_) {
                if (parent) {
                    grand.push_back(parent);
                }
            }
            return grand;
        }

        for (const auto& parent : person->parents_) {
            if (parent) {
                grand = findGrand(parent, n-1, type, grand);
            }
        }

    }

    return grand;
}

std::string Familytree::multiplyer(int n, std::string type) const
{
    //no need for "great-"
    std::string result;
    if (n ==1){
        return type;
    }

    for (int i =1; i<n;i++){
        result += "great-";
    }

    result += type;
    return result;
}
