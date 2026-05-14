#include "WebBuilder.h"
#include <map>
#include <algorithm>

int WebBuilder::find(std::vector<int>& parent, int x) {
    if (parent[x] != x)
        parent[x] = find(parent, parent[x]);
    return parent[x];
}

void WebBuilder::unite(std::vector<int>& parent, std::vector<int>& rank, int x, int y) {
    int rx = find(parent, x);
    int ry = find(parent, y);
    if (rx == ry) return;
    if (rank[rx] < rank[ry]) std::swap(rx, ry);
    parent[ry] = rx;
    if (rank[rx] == rank[ry]) rank[rx]++;
}

std::vector<Web> WebBuilder::buildWebs(const std::vector<LiveRange>& ranges) {
    // Group live ranges by variable name
    std::map<std::string, std::vector<int>> varToRanges;
    for (int i = 0; i < (int)ranges.size(); i++) {
        varToRanges[ranges[i].variable].push_back(i);
    }

    std::vector<Web> webs;
    int webId = 0;

    for (auto& [varName, rangeIndices] : varToRanges) {
        int n = rangeIndices.size();

        // Union-Find for these ranges
        std::vector<int> parent(n), rnk(n, 0);
        for (int i = 0; i < n; i++) parent[i] = i;

        // Merge ranges that share at least one line number
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                const auto& setA = ranges[rangeIndices[i]].lineSet;
                const auto& setB = ranges[rangeIndices[j]].lineSet;

                // Check for intersection
                bool overlap = false;
                auto it1 = setA.begin();
                auto it2 = setB.begin();
                while (it1 != setA.end() && it2 != setB.end()) {
                    if (*it1 == *it2) { overlap = true; break; }
                    if (*it1 < *it2) ++it1;
                    else ++it2;
                }
                if (overlap) {
                    unite(parent, rnk, i, j);
                }
            }
        }

        // Group ranges by their root in Union-Find
        std::map<int, std::vector<int>> groups;
        for (int i = 0; i < n; i++) {
            groups[find(parent, i)].push_back(i);
        }

        // Create a web for each group
        for (auto& [root, members] : groups) {
            Web web;
            web.id = webId++;
            web.variable = varName;

            for (int idx : members) {
                const LiveRange& lr = ranges[rangeIndices[idx]];
                for (const auto& pt : lr.points) {
                    web.lines.insert(pt.line);
                    // Merge annotations: if a point already exists, OR the flags
                    auto it = web.pointMap.find(pt.line);
                    if (it != web.pointMap.end()) {
                        it->second.isDef = it->second.isDef || pt.isDef;
                        it->second.isUse = it->second.isUse || pt.isUse;
                    } else {
                        web.pointMap[pt.line] = pt;
                    }
                }
            }

            webs.push_back(web);
        }
    }

    return webs;
}
