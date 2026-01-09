// Datastructures.cc

#include "datastructures.hh"

#include <random>
#include <set>
#include <queue>
#include <algorithm>
#include <memory>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures()
{
    // Write any initialization you need here

}

Datastructures::~Datastructures()
{
    // Write any cleanup you need here

}

bool Datastructures::add_beacon(BeaconID id, const Name& name, Coord xy, Color color)
{
    //if id already is there, av: O(1) wc: O(n) 
    if (beacons_.find(id) != beacons_.end()) {
        return false;
    };

    //add to the container, av: O(1) wc: O(n) 
    beacons_.emplace(id, Beacon{name, xy, color});
    return true;
}

int Datastructures::beacon_count()
{
    return static_cast<int>(beacons_.size());
}

void Datastructures::clear_beacons()
{
    beacons_.clear();
}

std::vector<BeaconID> Datastructures::all_beacons()
{
    std::vector<BeaconID> ids;
    ids.reserve(beacons_.size()); //reserve right ammount of space

    for (auto const& [id, beacon] : beacons_ ) {
        ids.push_back(id); //amortized O(1)
    };

    return ids;
}

Name Datastructures::get_name(BeaconID id)
{
    auto it = beacons_.find(id); //av: O(1) wc: O(n)
    if (it == beacons_.end()) {
        return NO_NAME;
    }
    return it->second.name;
}

Coord Datastructures::get_coordinates(BeaconID id)
{
   auto it = beacons_.find(id); //av: O(1) wc: O(n)
    if (it == beacons_.end()) {
        return NO_COORD;
    }
    return it->second.coord;
}

Color Datastructures::get_color(BeaconID id)
{
    auto it = beacons_.find(id); //av: O(1) wc: O(n)
    if (it == beacons_.end()) {
        return NO_COLOR;
    }
    return it->second.color;
}

std::vector<BeaconID> Datastructures::beacons_alphabetically()
{
    auto ids = all_beacons();
    std::sort(ids.begin(),ids.end(),[this](const BeaconID &a, const BeaconID &b){
        return beacons_.at(a).name < beacons_.at(b).name;
    });
    
    return ids;
}

std::vector<BeaconID> Datastructures::beacons_brightness_increasing()
{
    auto ids = all_beacons();
    std::sort(ids.begin(),ids.end(),[this](const BeaconID &a, const BeaconID &b){
        return calcBrightnes(beacons_.at(a).color) < calcBrightnes(beacons_.at(b).color);
    });
    return ids;
}

BeaconID Datastructures::min_brightness()
{
    if (beacons_.empty()){
        return NO_BEACON;
    }

    auto min_it = std::min_element(beacons_.begin(), beacons_.end(),
        [this](auto const& a, auto const& b){
            return calcBrightnes(a.second.color) < calcBrightnes(b.second.color);
        });
    
    //return the id of the beacon
    return min_it->first;
}

BeaconID Datastructures::max_brightness()
{
    if (beacons_.empty()){
        return NO_BEACON;
    }

    auto max_it = std::max_element(beacons_.begin(), beacons_.end(),
        [this](auto const& a, auto const& b){
            return calcBrightnes(a.second.color) < calcBrightnes(b.second.color);
        });
    
    //return the id of the beacon
    return max_it->first;
}

std::vector<BeaconID> Datastructures::find_beacons(Name const& name)
{
    std::vector<BeaconID> results;
    
    //get all given name beacon ids to result vector
    for (auto const& [id, beacon] : beacons_){
        if (beacon.name == name){
            results.push_back(id);
        }
    }
    //sort by id 
    std::sort (results.begin(), results.end());
    return results;
}

bool Datastructures::change_beacon_name(BeaconID id, const Name& newname)
{
    auto it = beacons_.find(id); //av: O(1) wc: O(n)
    if (it == beacons_.end()) {
        return false;
    }
    it->second.name = newname;
    return true;
}

bool Datastructures::add_lightbeam(BeaconID sourceid, BeaconID targetid)
{
    auto src = beacons_.find(sourceid);
    auto dst = beacons_.find(targetid);

    //no given src or dst beacon (O(1) on avg and O(n) on worst-case)
    if (src == beacons_.end() || dst == beacons_.end()){
        return false;
    };

    //already sending beam 
    if (src->second.target_id != NO_BEACON){
        return false;
    };

    src->second.target_id = targetid;
    dst->second.in_beams.push_back(sourceid);
    return true;
}

std::vector<BeaconID> Datastructures::get_lightsources(BeaconID id)
{
    auto it = beacons_.find(id);
    if (it == beacons_.end()){
        return { NO_BEACON };
    }
    auto sources = it->second.in_beams;
    std::sort(sources.begin(),sources.end());
    return sources;
}

std::vector<BeaconID> Datastructures::path_outbeam(BeaconID id)
{
    std::vector<BeaconID> path;

    auto it = beacons_.find(id);
    if (it == beacons_.end()){
        return { NO_BEACON };
    }

    BeaconID current = id;
    while (current != NO_BEACON){
        path.push_back(current);
        auto next_it = beacons_.find(current);
        if (next_it != beacons_.end() || next_it->second.target_id != NO_BEACON){
            current = next_it->second.target_id;
        }
    }
    return path;
}

std::vector<BeaconID> Datastructures::path_inbeam_longest(BeaconID id)
{
    std::unordered_map<BeaconID, std::vector<BeaconID>> memo;
    std::unordered_set<BeaconID> visiting;
    return dfs_longest(id, memo, visiting);
}

Color Datastructures::total_color(BeaconID id)
{
    auto it = beacons_.find(id);
    if (it == beacons_.end()) {
        return NO_COLOR;
    }
    const auto& beacon = it->second;
    
    //values from beacon
    int sum_r = beacon.color.r;
    int sum_g = beacon.color.g;
    int sum_b = beacon.color.b;
    int count = 1;

    //go through all incoming beams and ad to sum 
    for (const auto& src_id : beacon.in_beams){
        Color src_color = total_color(src_id);
        if (src_color == NO_COLOR) continue;
        sum_r += src_color.r;
        sum_g += src_color.g;
        sum_b += src_color.b;
        ++count;
    }

    Color avg;
    avg.r = sum_r/count;
    avg.g = sum_g/count;
    avg.b = sum_b/count;
    return avg;
}

bool Datastructures::add_fibre(Coord xpoint1, Coord xpoint2, Cost cost)
{
    if (xpoint1 == xpoint2){
        return false;
    }

    //ensure there is not already these nodes and create them
    if (graph_.find(xpoint1) == graph_.end()) {
        graph_[xpoint1] = std::make_shared<Node>();
        graph_[xpoint1]->coord = xpoint1;
    }
    if (graph_.find(xpoint2) == graph_.end()) {
        graph_[xpoint2] = std::make_shared<Node>();
        graph_[xpoint2]->coord = xpoint2;
    }
    
    
    auto& adj1 = graph_[xpoint1]->adj;
    auto& adj2 = graph_[xpoint2]->adj;

    //check if fibre already exist
    if (adj1.find(xpoint2) != adj1.end()) return false;

    //add the fibre in poth directions
    adj1[xpoint2] = cost;
    adj2[xpoint1] = cost;
    return true;
}

std::vector<Coord> Datastructures::all_xpoints()
{
    std::vector<Coord> points;
    points.reserve(graph_.size());
    for (const auto& [coord, node] : graph_){
        points.push_back(coord);
    }

    std::sort(points.begin(),points.end());
    return points;
}

std::vector<std::pair<Coord, Cost> > Datastructures::get_fibres_from(Coord xpoint)
{
    std::vector<std::pair<Coord, Cost>> result;
    auto it = graph_.find(xpoint);
    if (it == graph_.end()) return result;

    for (const auto& [dest, cost] : it->second->adj){
        result.push_back({dest, cost});
    }
    std::sort(result.begin(),result.end(),[](auto const& a, auto const& b) {
            return a.first < b.first;});
    return result;
}

std::vector<std::pair<Coord, Coord> > Datastructures::all_fibres()
{
    std::vector<std::pair<Coord, Coord>> fibers;
    std::set<std::pair<Coord, Coord>> seen;

    for (const auto& [from, node] : graph_){
        for (const auto& [to, cost] : node->adj){
            Coord c1 = from < to ? from : to;
            Coord c2 = from < to ? to : from;

            std::pair<Coord, Coord> key = {c1,c2};

            //set insert.second returns false if given pair is already exists
            if (seen.insert(key).second){
                fibers.push_back(key);
            }
        }
    }

    //sort by mainly by first coord and secondy by second coord
    std::sort(fibers.begin(), fibers.end(), [](auto const& a, auto const& b){
        return (a.first < b.first) || (!(b.first < a.first) && a.second < b.second); 
    });
    return fibers;
}

bool Datastructures::remove_fibre(Coord xpoint1, Coord xpoint2)
{
    auto it1 = graph_.find(xpoint1);
    auto it2 = graph_.find(xpoint2);

    if (it1 == graph_.end() || it2 == graph_.end()){
        return false;
    }

    bool removed = false;

    //remove xpoin2 
    auto& adj1 = it1->second->adj;
    if (adj1.erase(xpoint2) == 1) {
        removed = true;}

    auto& adj2 = it2->second->adj;
    if (adj2.erase(xpoint1) == 1) {
        removed = true;}

    return removed;
}

void Datastructures::clear_fibres()
{
    graph_.clear();
}

std::vector<std::pair<Coord, Cost> > Datastructures::route_any(Coord fromxpoint, Coord toxpoint)
{
    std::vector<std::pair<Coord, Cost>> path;

    //check that both nodes are legit
    auto it1 = graph_.find(fromxpoint);
    auto it2 = graph_.find(toxpoint);
    if (it1 == graph_.end() || it2 == graph_.end()){
        return path;
    }

    resetNodes();

    //BFS queue
    std::queue<std::shared_ptr<Node>> Q;

    //init starting node
    auto s = it1->second;
    s->colour = Colour::GRAY;
    s->d = 0;

    Q.push(s);

    bool found = false;
    while (!Q.empty()){
        auto u = Q.front();
        Q.pop();
        
        if (u->coord == toxpoint){
            found = true;
            break;
        }

        for (auto& [vcoord, cost] : u->adj){
            auto v = graph_.find(vcoord)->second;
            if (v->colour == Colour::WHITE){
                v->colour = Colour::GRAY;
                v->d = u->d+cost;
                v->pi = u.get(); 
                Q.push(v);
            }
        }
        u->colour = Colour::BLACK;
    }

    if (!found){
        return path;
    }

    //create path (wrongway)
    std::vector<Node*> reverse_path;
    Node* current = graph_[toxpoint].get();
    reverse_path.reserve(current->d);
    while (current != nullptr) {
        reverse_path.push_back(current);
        current = current->pi;
    }

    std::reverse(reverse_path.begin(),reverse_path.end());
    path.reserve(reverse_path.size());

    for (const Node* n : reverse_path) {
        path.emplace_back(n->coord, n->d);
    }

    return path;
}

std::vector<std::pair<Coord, Cost>> Datastructures::route_least_xpoints(Coord fromxpoint, Coord toxpoint)
{
   return route_any(fromxpoint,toxpoint);
}

std::vector<std::pair<Coord, Cost>> Datastructures::route_fastest(Coord fromxpoint, Coord toxpoint)
{
    std::vector<std::pair<Coord, Cost>> path;

    //check that both nodes are legit
    auto it1 = graph_.find(fromxpoint);
    auto it2 = graph_.find(toxpoint);
    if (it1 == graph_.end() || it2 == graph_.end()){
        return path;
    }

    resetNodes();

    //prioriti queue (min heap)
    using PQItem = std::pair<Cost, std::shared_ptr<Node>>; // (distance, node)
    auto cmp = [](const PQItem& a, const PQItem& b) { return a.first > b.first; };
    std::priority_queue<PQItem, std::vector<PQItem>, decltype(cmp)> Q(cmp);
    

    //init starting node
    auto s = it1->second;
    s->colour = Colour::GRAY;
    s->d = 0;

    //push start node to the list
    Q.push({s->d,s});

    bool found = false;

    //dijkstra loop
    while (!Q.empty()){
        //extract min
        auto [u_de, u] = Q.top();
        Q.pop();

        if (u->colour == Colour::BLACK) continue;

        //if to xpoint is found end
        if (u->coord == toxpoint){
                found = true;
                break;
        }

        for (const auto& [vcoord, cost] : u->adj){
            auto v = graph_.find(vcoord)->second;
            Cost new_cost = u->d + cost;
            if (new_cost < v->d) {
                v->d = new_cost;
                v->pi = u.get();
                if (v->colour != Colour::BLACK) {
                    v->colour = Colour::GRAY;
                    Q.push({v->d, v});
                }
            }
        }
        u->colour = Colour::BLACK;
    }

    if (!found){
        return path;
    }

    std::vector<Node*> reverse_path;
    Node* current = graph_[toxpoint].get();
    while (current != nullptr) {
        reverse_path.push_back(current);
        current = current->pi;
    }

    std::reverse(reverse_path.begin(),reverse_path.end());
    path.reserve(reverse_path.size());

    for (const Node* n : reverse_path) {
        path.emplace_back(n->coord, n->d);
    }
    return path; 
}

std::vector<Coord> Datastructures::route_fibre_cycle(Coord startxpoint)
{
    auto it = graph_.find(startxpoint);
    if (it == graph_.end()) {
        return {};
    }
    resetNodes();
    return dfs_cycle(it->second);
}
    

//here starts own helper functions
int Datastructures::calcBrightnes(const Color &color)
{
    return 3*color.r + 6*color.g + color.b;
}

std::vector<BeaconID> Datastructures::dfs_longest(BeaconID id, std::unordered_map<BeaconID, std::vector<BeaconID>> &memo, std::unordered_set<BeaconID> &visiting)
{
    if (beacons_.find(id) == beacons_.end()) {
        return { NO_BEACON };
    }

    // return value in memo
    if (memo.find(id) != memo.end()) {
        return memo[id];
    }

    visiting.insert(id);  // mark as currenty visited
    const auto& beacon = beacons_.at(id);
    std::vector<BeaconID> longest_path;

    // if no incoming beams
    if (beacon.in_beams.empty()) {
        longest_path = { id };
    } else {
        // try all in beams
        for (BeaconID src : beacon.in_beams) {
            auto path = dfs_longest(src, memo, visiting);
            if (path.size() > longest_path.size()) {
                longest_path = std::move(path);
            }
        }
        longest_path.push_back(id);
    }

    visiting.erase(id);    
    memo[id] = longest_path; 
    return memo[id];
}

void Datastructures::resetNodes()
{
    //reset all nodes
    for (const auto& [coord, node] : graph_){
        node->reset();
    }
}

std::vector<Coord> Datastructures::dfs_cycle(std::shared_ptr<Node> s)
{
    s->colour = Colour::GRAY;

    for (const auto& [vcoord, cost] : s->adj) {
        auto v = graph_.find(vcoord)->second;

        if (v->colour == Colour::WHITE) {
            v->pi = s.get();

            auto cycle = dfs_cycle(v);
            if (!cycle.empty()) {
                return cycle;
            }
        }
        else if (v->colour == Colour::GRAY && (!s->pi || v->coord != s->pi->coord)) {
            // cycle found
            std::vector<Coord> reverse_cycle;
            reverse_cycle.push_back(v->coord);
            Node* current = s.get();
            while (current != nullptr && current->coord != v->coord) {
                reverse_cycle.push_back(current->coord);
                current = current->pi;
            }
            reverse_cycle.push_back(v->coord);
            std::reverse(reverse_cycle.begin(), reverse_cycle.end());
            return reverse_cycle;
        }
    }

    s->colour = Colour::BLACK;
    return {};
}
