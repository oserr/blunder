namespace blunder {

#include <memory>

// Blunder brings everything together:
// - Move generation
// - Position evaluation
// - Search
//
// For now, the idea is that Blunder will manage a single game. If we develop a
// server for blunder to play multiple simulataneous game, then the idea is that
// the serve would spawn a new instance of Blunder to play each game. For simple
// tasks, we might be able to get away with using Blunder to handle multiple
// requests, however, the ideal scenario is that Blunder is able to use all of
// the machine for a single game, especially a blitz game where we want to take
// advantage of every millisecond.
class Blunder {
public:

private:
};

} // namespace blunder
