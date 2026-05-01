#include <cstddef>
#include <tuple>
#include <utility>

template< class T >
struct BiTree
{
  T val;
  BiTree< T >* lt;
  BiTree< T >* rt;
  BiTree< T >* parent;
};

enum class Move
{
  FallLeft,
  Parent
};

template< class T >
std::pair< std::size_t, BiTree< T >* > fallLeft(BiTree< T >* node)
{
  std::size_t count = 0;

  while (node && node->lt)
  {
    node = node->lt;
    ++count;
  }

  return {count, node};
}

template< class T >
BiTree< T >* rightmost(BiTree< T >* node)
{
  if (!node)
  {
    return nullptr;
  }

  while (node->rt)
  {
    node = node->rt;
  }

  return node;
}

template< class T >
bool isInsideSubtree(BiTree< T >* root, BiTree< T >* node)
{
  while (node)
  {
    if (node == root)
    {
      return true;
    }

    node = node->parent;
  }

  return false;
}

template< class T >
std::tuple< Move, std::size_t, BiTree< T >* > nextStruct(
  BiTree< T >* root,
  BiTree< T >* node
)
{
  if (!root || !node || !isInsideSubtree(root, node))
  {
    return {Move::Parent, 0, nullptr};
  }

  if (node->rt)
  {
    std::pair< std::size_t, BiTree< T >* > res = fallLeft(node->rt);
    return {Move::FallLeft, res.first + 1, res.second};
  }

  std::size_t count = 1;
  BiTree< T >* child = node;
  BiTree< T >* parent = node->parent;

  while (parent && isInsideSubtree(root, parent) && parent->rt == child)
  {
    child = parent;
    parent = parent->parent;
    ++count;
  }

  if (parent && isInsideSubtree(root, parent))
  {
    return {Move::Parent, count, parent};
  }

  return {Move::Parent, count, nullptr};
}

template< class T >
bool sameStep(
  const std::tuple< Move, std::size_t, BiTree< T >* >& lhs,
  const std::tuple< Move, std::size_t, BiTree< T >* >& rhs
)
{
  return std::get< 0 >(lhs) == std::get< 0 >(rhs) &&
         std::get< 1 >(lhs) == std::get< 1 >(rhs);
}

template< class T >
std::tuple< BiTree< T >*, BiTree< T >*, bool > isEqualStructStart(
  BiTree< T >* lhsRoot,
  BiTree< T >* lhsStart,
  BiTree< T >* rhsRoot,
  BiTree< T >* rhsStart
)
{
  if (!lhsStart || !rhsStart)
  {
    return {lhsStart, rhsStart, lhsStart == rhsStart};
  }

  std::tuple< Move, std::size_t, BiTree< T >* > lhsNext =
    nextStruct(lhsRoot, lhsStart);

  std::tuple< Move, std::size_t, BiTree< T >* > rhsNext =
    nextStruct(rhsRoot, rhsStart);

  while (
    sameStep(lhsNext, rhsNext) &&
    std::get< 2 >(lhsNext) &&
    std::get< 2 >(rhsNext)
  )
  {
    lhsNext = nextStruct(lhsRoot, std::get< 2 >(lhsNext));
    rhsNext = nextStruct(rhsRoot, std::get< 2 >(rhsNext));
  }

  return {
    std::get< 2 >(lhsNext),
    std::get< 2 >(rhsNext),
    sameStep(lhsNext, rhsNext)
  };
}

template< class T >
bool isEqualStruct(BiTree< T >* lhs, BiTree< T >* rhs)
{
  if (!lhs && !rhs)
  {
    return true;
  }

  if (!lhs || !rhs)
  {
    return false;
  }

  std::pair< std::size_t, BiTree< T >* > lhsBegin = fallLeft(lhs);
  std::pair< std::size_t, BiTree< T >* > rhsBegin = fallLeft(rhs);

  if (lhsBegin.first != rhsBegin.first)
  {
    return false;
  }

  std::tuple< BiTree< T >*, BiTree< T >*, bool > result =
    isEqualStructStart(lhs, lhsBegin.second, rhs, rhsBegin.second);

  return !std::get< 0 >(result) &&
         !std::get< 1 >(result) &&
         std::get< 2 >(result);
}

template< class T >
struct InclusionSearchResult
{
  std::pair< BiTree< T >*, BiTree< T >* > range;
  BiTree< T >* root;
};

template< class T >
InclusionSearchResult< T > inclusionFrom(
  BiTree< T >* lhsRoot,
  BiTree< T >* from,
  BiTree< T >* pattern
)
{
  if (!lhsRoot || !from || !pattern)
  {
    return {{nullptr, nullptr}, nullptr};
  }

  BiTree< T >* curr = from;

  while (curr)
  {
    if (isEqualStruct(curr, pattern))
    {
      return {{fallLeft(curr).second, rightmost(curr)}, curr};
    }

    curr = std::get< 2 >(nextStruct(lhsRoot, curr));
  }

  return {{nullptr, nullptr}, nullptr};
}

template< class T >
std::pair< BiTree< T >*, BiTree< T >* > inclusion(
  BiTree< T >* lhs,
  BiTree< T >* pattern
)
{
  if (!lhs || !pattern)
  {
    return {nullptr, nullptr};
  }

  InclusionSearchResult< T > result =
    inclusionFrom(lhs, fallLeft(lhs).second, pattern);

  return result.range;
}

template< class T >
bool includedStruct(BiTree< T >* lhs, BiTree< T >* pattern)
{
  if (!pattern)
  {
    return true;
  }

  if (!lhs)
  {
    return false;
  }

  return inclusion(lhs, pattern).first != nullptr;
}

template< class T >
struct InclusionIt
{
  std::pair< BiTree< T >*, BiTree< T >* > incl;
  BiTree< T >* lhsRoot;
  BiTree< T >* matchRoot;
};

template< class T >
InclusionIt< T > begin(BiTree< T >* lhs, BiTree< T >* pattern)
{
  if (!lhs || !pattern)
  {
    return {{nullptr, nullptr}, lhs, nullptr};
  }

  InclusionSearchResult< T > result =
    inclusionFrom(lhs, fallLeft(lhs).second, pattern);

  return {result.range, lhs, result.root};
}

template< class T >
InclusionIt< T > next(InclusionIt< T > curr, BiTree< T >* pattern)
{
  if (!curr.lhsRoot || !curr.matchRoot || !pattern)
  {
    return {{nullptr, nullptr}, curr.lhsRoot, nullptr};
  }

  BiTree< T >* from =
    std::get< 2 >(nextStruct(curr.lhsRoot, curr.matchRoot));

  InclusionSearchResult< T > result =
    inclusionFrom(curr.lhsRoot, from, pattern);

  return {result.range, curr.lhsRoot, result.root};
}

template< class T >
bool hasNext(InclusionIt< T > curr, BiTree< T >* pattern)
{
  InclusionIt< T > nextIt = next(curr, pattern);
  return nextIt.incl.first != nullptr;
}
