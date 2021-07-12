#include <iostream>
#include <vector>
#include <limits>
#include <functional>
#include <fstream>
using namespace std;

template <class T>
struct array2d_t
{
    int n, m;
    vector<T> v;
    static const T NINF = numeric_limits<T>::min();

    array2d_t(int n, int m, T val = NINF) : n(n), m(m)
    {
        v.resize(n * m, val);
    }

    T &operator()(int x, int y)
    {
        return v[x * m + y];
    }

    const T &operator()(int x, int y) const
    {
        return v[x * m + y];
    }

    array2d_t transpose() const
    {
        array2d_t D(m, n);
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < m; ++j)
                D(j, i) = (*this)(i, j);
        return D;
    }
};

using array2d = array2d_t<int>;

struct tree
{
    tree(const char* filename)
    {
        ifstream fin(filename);
        if (!fin)
        {
            cerr << "unable to read " << filename << "\n";
            exit(EXIT_FAILURE);
        }

        fin >> n;
        vector<vector<int>> g(n);
        parent.resize(n);
        children.resize(n);
        for (int i = 1; i < n; ++i)
        {
            int u, v;
            fin >> u >> v;
            g[u].push_back(v);
            g[v].push_back(u);
        }

        function<void(int,int)> dfs = [&](int x, int p)
        {
            parent[x] = p;
            for (int y : g[x])
            {
                if (y != p)
                {
                    children[x].push_back(y);
                    dfs(y, x);
                }
            }
        };
        dfs(0, -1);
    }

    int n;
    vector<int> parent;
    vector<vector<int>> children;
};

array2d read_matrix(const char* filename)
{
    ifstream fin(filename);
    if (!fin)
    {
        cerr << "unable to read " << filename << "\n";
        exit(EXIT_FAILURE);
    }
    int n, m;
    cin >> n >> m;
    array2d mat(n, m);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            fin >> mat(i, j);
    return mat;
}

int tree_path(const tree &t1, // tree
              const tree &t2, // path
              const array2d &m, // weight matrix
              array2d& dp, // memoization
              int x, // tree root
              const int u, // path root
              int v) // path leaf
{
    if (dp(x, v) != array2d::NINF)
        return dp(u, v);

    int tree_step = m(x, v);
    for (int y : t1.children[x])
        tree_step += tree_path(t1, t2, m, dp, y, u, v);

    int &solution = dp(x, v);
    if (t2.parent[v] != u)
    {
        int path_step = m(x, v) + tree_path(t1, t2, m, dp, x, u, t2.parent[v]);
        solution = max(tree_step, path_step);
    }
    else
        solution = tree_step;

    return solution;
}

int tree_tree(const tree &t1, // antichain tree
              const tree &t2, // path tree
              const array2d &m, // weight matrix
              const array2d &mt, // transpose weight matrix
              array2d &dp, // antichain-path dp table
              array2d &dpt, // path-antichain dp table
              vector<array2d> &gamma, // tree-path dp table
              vector<array2d> &gammat, // path-tree dp table
              int u, // t1 root
              int v) // t2 root
{
    if (dp(u, v) != array2d::NINF)
        return dp(u, v);

    // select antichain
    function<int(int, int)> dfs2 = [&](int y, int vp)
    {
        if (t2.children[vp].empty())
            return tree_path(t1, t2, m, gamma[u], u, v, vp);

        int solution = 0;
        int tree_path_sum = 0;
        for (int up : t1.children[y])
        {
            for (int z : t1.children[t1.parent[up]])
                if (z != up)
                    tree_path_sum += tree_path(t1, t2, m, gamma[v], z, v, vp);

            solution = max(solution, tree_path_sum + tree_tree(t2, t1, mt, m, dpt, dp, gammat, gamma, vp, up));
            solution = max(solution, dfs2(up, vp));

            for (int z : t1.children[t1.parent[up]])
                if (z != up)
                    tree_path_sum -= tree_path(t1, t2, m, gamma[v], z, v, vp);
        }
        return solution;
    };

    // select path
    function<int(int)> dfs1 = [&](int x)
    {
        int solution = 0;
        for (int vp : t2.children[x])
        {
            solution = max(solution, dfs2(u, vp));
            solution = max(solution, dfs1(vp));
        }
        return solution;
    };
    return dp(u, v) = dfs1(v);
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        cerr << "usage: " << argv[0] << " <tree1> <tree2> <matrix>\n";
        return EXIT_FAILURE;
    }
    tree t1(argv[1]);
    tree t2(argv[2]);
    array2d m = read_matrix(argv[3]); // weight matrix
    array2d mt = m.transpose(); // transpose weight matrix
    array2d dp(t1.n, t2.n); // antichain-path dp table
    array2d dpt(t2.n, t1.n); // path-antichain dp table
    vector<array2d> gamma(t2.n, array2d(t1.n, t2.n)); // tree-path dp table
    vector<array2d> gammat(t1.n, array2d(t2.n, t1.n)); // path-tree dp table
    int solution = min(tree_tree(t1, t2, m, mt, dp, dpt, gamma, gammat, 0, 0),
                       tree_tree(t2, t1, mt, m, dpt, dp, gammat, gamma, 0, 0));
    cout << solution << endl;
}
