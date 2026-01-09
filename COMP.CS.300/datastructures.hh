// Datastructures.hh

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <utility>
#include <limits>
#include <source_location>
#include <unordered_map>
#include <unordered_set>
#include <memory>

// Type for beacon IDs
using BeaconID = std::string;
using Name = std::string;

// Return value for cases where required beacon was not found
BeaconID const NO_BEACON= "--NO_BEACON--";

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Return value for cases where name values were not found
Name const NO_NAME = "-- NO_NAME --";

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};

// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

struct CoordHash
{
    std::size_t operator()(Coord xy) const
    {
        auto hasher = std::hash<int>();
        auto xhash = hasher(xy.x);
        auto yhash = hasher(xy.y);
        // Combine hash values (magic!)
        return xhash ^ (yhash + 0x9e3779b9 + (xhash << 6) + (xhash >> 2));
    }
};

// Example: Defining < for Coord so that it can be used
// as key for std::map/set
inline bool operator<(Coord c1, Coord c2)
{
    if (c1.y < c2.y) { return true; }
    else if (c2.y < c1.y) { return false; }
    else { return c1.x < c2.x; }
}

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

// Type for color (RGB)
struct Color
{
    int r = NO_VALUE;
    int g = NO_VALUE;
    int b = NO_VALUE;
};

// Equality and non-equality comparisons for Colors
inline bool operator==(Color c1, Color c2) { return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b; }
inline bool operator!=(Color c1, Color c2) { return !(c1==c2); }

// Return value for cases where color was not found
Color const NO_COLOR = {NO_VALUE, NO_VALUE, NO_VALUE};

// Type for light transmission cost (used only in the second assignment)
using Cost = int;

// Return value for cases where cost is unknown
Cost const NO_COST = NO_VALUE;

// This exception class is there just so that the user interface can notify
// about operations which are not (yet) implemented
class NotImplemented : public std::exception
{
public:
    explicit NotImplemented(std::string const msg = "",
                            const std::source_location location = std::source_location::current())
        : msg_{}
    {
        std::string funcname = location.function_name();
        if (auto namestart = funcname.find_last_of(':'); namestart != std::string::npos)
        { funcname.erase(0, namestart+1); }
        if (auto nameend = funcname.find_first_of('('); nameend != std::string::npos)
        { funcname.erase(nameend, std::string::npos); }
        msg_ = (!msg.empty() ? msg+" in " : "")+funcname+"()";
    }
    virtual const char* what() const noexcept override
    {
        return msg_.c_str();
    }
private:
    std::string msg_;
};

// This is the class you are supposed to implement

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // A operations

    // adds beacon to the container
    // return: false if already exists, true if added 
    // Estimate of performance: O(1) average, O(n) worst-case (hash collisions)
    // Short rationale for estimate: unordered_map lookup and insertion
    bool add_beacon(BeaconID id, Name const& name, Coord xy, Color color);

    // returns size of the container
    // Estimate of performance: O(1)
    // Short rationale for estimate: unordered_map::size() is always constant
    // Note: there is possibility of overflow with very large data sizes
    // because conversion from size_t to int
    int beacon_count();

    // clears all beacons
    // Estimate of performance: O(n)
    // Short rationale for estimate: unordered_map::clear() iterates through
    // all elements to destruct them
    void clear_beacons();

    // copies all beacon ids to the vector and returns it
    // Estimate of performance: O(n)
    // Short rationale for estimate: goes trough all elements
    // and copies them to the vector (vector.push_back amortized O(1))
    std::vector<BeaconID> all_beacons();

    // returns: name of given id if possible, else returns NO_NAME
    // Estimate of performance: O(1) average, O(n) worst-case (hash collisions)
    // Short rationale for estimate: unordered_map lookup by key
    Name get_name(BeaconID id);

    // returns: coordinates of given id if possible, else returns NO_COORD
    // Estimate of performance: O(1) average, O(n) worst-case (hash collisions)
    // Short rationale for estimate: unordered_map lookup by key
    Coord get_coordinates(BeaconID id);

    // returns: color of given id if possible, else returns NO_COLOR
    // Estimate of performance: O(1) average, O(n) worst-case (hash collisions)
    // Short rationale for estimate: unordered_map lookup by key
    Color get_color(BeaconID id);

    // We recommend you implement the operations below only after implementing the ones above

    // returns: vector where beacon ids are in alphabeticly ordered by name
    // Estimate of performance: O(n log n)
    // Short rationale for estimate: all_beacons is O(n), sorting the vector
    // of n elements by beacon name takes O(n log n) time.
    std::vector<BeaconID> beacons_alphabetically();

    // returns: vector where beacon ids are in sorted increasing by brightnes 
    // Estimate of performance: O(n log n)
    // Short rationale for estimate: all_beacons is O(n), sorting the vector
    // of n elements by beacon name takes O(n log n) time.
    std::vector<BeaconID> beacons_brightness_increasing();

    // Estimate of performance: O(n)
    // Short rationale for estimate: Linear scan through all beacons
    // to find smallest brightness using std::min_element
    BeaconID min_brightness();

    // Estimate of performance: O(n)
    // Short rationale for estimate: Linear scan through all beacons
    // to find max brightness using std::max_element
    BeaconID max_brightness();

    // Estimate of performance: O(n + m log m), where n = total beacons, m = matches
    // Short rationale for estimate:Linear scan for matches O(n),
    // then sort the m matches O(m log m)
    std::vector<BeaconID> find_beacons(Name const& name);

    // Estimate of performance: O(1) average, O(n) worst-case (hash collisions)
    // Short rationale for estimate: unordered_map lookup and assignment
    bool change_beacon_name(BeaconID id, Name const& newname);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: O(1) average, O(n) worst-case (hash collisions)
    // Short rationale for estimate: two unordered map look ups 
    //and push_back to vector
    bool add_lightbeam(BeaconID sourceid, BeaconID targetid);

    // Estimate of performance: O(s log s), s = number of sources
    // Short rationale for estimate: map find is O(1) on avg 
    // sorting the lightsources ids (s) takes O(s log s)
    std::vector<BeaconID> get_lightsources(BeaconID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: linear travelsal of the beam chain
    // -> in worst case it goes through all beacons
    std::vector<BeaconID> path_outbeam(BeaconID id);

    // B operations

    // Estimate of performance: O(N + E), 
    // where N is the number of beacons and E is the number of in-beams
    // Short rationale for estimate:DFS visits each beacon once
    //iterates all beams per beacon, memo keeps tarck of visited beacons
    std::vector<BeaconID> path_inbeam_longest(BeaconID id);

    // Estimate of performance: O(N + E), 
    // where N is the number of beacons and E is the number of in-beams
    // Short rationale for estimate: goes through all beams once
    Color total_color(BeaconID id);

    //adds fibre connection between too given xpoints with given cost
    //returns false if x point are same or fibre between already exists
    // Estimate of performance: O(1) average, O(n) worst-case
    // Short rationale for estimate: unordered_map lookup and insertion
    bool add_fibre(Coord xpoint1, Coord xpoint2, Cost cost);

    // returns all xpoints (Coord) that exist in the fibre network, sorted asc
    // Estimate of performance: O(n log n)
    // Short rationale for estimate:Collect all keys O(n), sort O(n log n)
    std::vector<Coord> all_xpoints();

    // returns all fibers (dest and cost) connected to given xpoint
    // returns empty vec if xpoint has no fibers
    // Estimate of performance: O(m log m), m = number of fibres
    // Short rationale for estimate: Short rationale: Copy adjacency list and sort
    std::vector<std::pair<Coord, Cost>> get_fibres_from(Coord xpoint);

    // returns all fibre connections once, sorted
    // Estimate of performance:O(E log E), E = total fibres
    // Short rationale: Iterates all edges, inserts into set, sorts result
    std::vector<std::pair<Coord, Coord>> all_fibres();

    // Removes the fibre connection between to xpoints, if it exist
    // returns true if removed false otherwise (fibre doestn exist)
    // Estimate of performance: O(1) average, O(n) worst-case
    // Short rationale for estimate:unordered_map lookup and erase
    bool remove_fibre(Coord xpoint1, Coord xpoint2);

    // clears all fibres
    // Estimate of performance: O(1);
    // Short rationale for estimate: unordered_map::clear is O(1)
    void clear_fibres();

    // We recommend you implement the operations below
    // only after implementing the ones above

    /// finds any route between two xpoints (BFS)
    // Estimate of performance: O(V+E)
    // Short rationale: BFS visits each node and edge onces
    std::vector<std::pair<Coord, Cost>> route_any(Coord fromxpoint, Coord toxpoint);

    // C operations

    // finds route with least xpoints
    // Estimate of performance: O(V+E)
    // Short rationale for estimate:BFS traversal route_any
    std::vector<std::pair<Coord, Cost>> route_least_xpoints(Coord fromxpoint, Coord toxpoint);

    // finds fastest route using Dijkstra
    // Estimate of performance: O((V + E) log V)
    // Short rationale: Dijkstra’s algorithm with priority queue
    std::vector<std::pair<Coord, Cost>> route_fastest(Coord fromxpoint, Coord toxpoint);

    // finds a cycle starting at xpoint
    // Estimate of performance: O(V + E)
    // Short rationale for estimate: DFS traversal of the graph
    std::vector<Coord> route_fibre_cycle(Coord startxpoint);

private:
    /*
    The main data structures chosen for this class are:
     std::unordered_map,
     custom structs (Beacon, Node).

    -std::unordered_map: is used for storing beacons (beacons_) and the fibre network graph (graph_) and nodes adj
    provides average-case constant time (O(1)) for insertions, deletions, and lookups.

    -Beacon struct encapsulates all relevant data for a beacon,
    making the code more organized and easier to maintain.

    -Node struct represents a node in the fibre network,
    storing its values (coord, adj),
    and temporary fields for pathfinding algorithms (color, distance, parent pointer).
    This design allows efficient graph traversal and modification.
    */

    struct Beacon
    {
        Name name;
        Coord coord;
        Color color;
        BeaconID target_id = NO_BEACON;
        std::vector<BeaconID> in_beams = {};  
    };

    enum class Colour { WHITE, GRAY, BLACK };

    struct Node
    {
        Coord coord;
        std::unordered_map<Coord, Cost,CoordHash>adj;
        
        //these are only for pathfinding algorithms
        Colour colour = Colour::WHITE;
        Node* pi = nullptr;
        Cost d = std::numeric_limits<Cost>::max(); //inf
        

        void reset() {
            colour = Colour::WHITE;
            d = std::numeric_limits<Cost>::max();
            pi = nullptr;
        }
    };

    //datastructures
    std::unordered_map<BeaconID,Beacon> beacons_;
    std::unordered_map<Coord,std::shared_ptr<Node>, CoordHash> graph_;
 
    // calculates brightness from given color
    // brightnes is calculated using (3*r+6*g+b)
    int calcBrightnes (const Color &color);

    //dfs that finds longest paths
    std::vector<BeaconID> dfs_longest(BeaconID id,
                    std::unordered_map<BeaconID, std::vector<BeaconID>>& memo,
                    std::unordered_set<BeaconID> &visiting);
    
    //dfs that detects cycles (rekursive)
    std::vector<Coord> dfs_cycle(std::shared_ptr<Node> s);
    
    //resets nodes temp pathfinding fields
    void resetNodes();                 
};

#endif // DATASTRUCTURES_HH
