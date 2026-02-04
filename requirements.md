### **Project: Comparative Analysis of Priority Queues in Graph Algorithms**

## Overview

In this project, you will implement and evaluate priority queue data structures in the context of two fundamental graph algorithms:

- Dijkstra’s Shortest Path Algorithm
- Prim’s Minimum Spanning Tree Algorithm

Your goal is to understand not only the theoretical complexity guarantees of different heaps, but also their practical performance, implementation complexity, and trade-offs.

You will focus on two advanced priority queue implementations:

- Fibonacci Heaps
- Pairing Heaps

## Learning Objectives

By completing this project, you will:

- Deepen your understanding of priority queue abstractions
- Analyze the impact of decrease-key and extract-min on algorithm performance
- Contrast amortized theoretical bounds with empirical results
- Gain experience implementing complex data structures
- Practice collaborative software development and integration
- Develop skills in experimental algorithm analysis

## Team Structure

You will work in groups of four.

Each group member should take primary responsibility for one of the following roles:

1. Graph Algorithms Lead

2. Fibonacci Heap Lead

3. Pairing Heap Lead

4. Evaluation & Analysis Lead

All group members are expected to understand the full system and contribute to final integration and reporting.

## Part 1: Algorithm Implementations

You must implement:

- Dijkstra’s Algorithm (single-source shortest paths, non-negative weights)
- Prim’s Algorithm (minimum spanning tree)

Implementations must be modular so that priority queue implementations can be swapped without changing algorithm logic.

## Part 2: Heap Implementations

Required heaps:

- Fibonacci Heap
- Pairing Heap

Optional:

- Binary heap (baseline)

Each heap must support:

- insert
- extract-min
- decrease-key
- find-min (if separate)

## Part 3: Experimental Design

You will conduct a comparative performance analysis across:

- Algorithm (Dijkstra vs Prim)
- Heap type (Fibonacci vs Pairing)
- Graph characteristics (sparse vs dense, small vs large)

Suggested graph types:

- Random graphs
- Grid graphs
- Synthetic worst-case graphs

Document all parameters clearly.

## Part 4: Metrics

Evaluate:

- Total runtime
- Time spent in extract-min and decrease-key
- Number of heap operations
- Memory usage (qualitative acceptable)
- Implementation complexity

## Part 5: Analysis and Discussion

Your report should address:

- Do Fibonacci heaps provide practical benefits?
- How do pairing heaps compare in practice?
- Which algorithm benefits more from advanced heaps?
- How graph structure affects performance?
- Why theory and practice differ

## Deliverables

Submit:

1. Source code

2. Written report (8–12 pages recommended)

~~3. Short presentation (10–12 minutes)~~

### Evaluation Criteria

Correctness – 30%

Experimental Design – 20%

Analysis & Insight – 25%

Code Quality & Modularity – 15%

Writing ~~& Presentation~~ – 10%

### Replacement Rubric

| **Component** | **Criteria** | **Marks** |
| --- | --- | --- |
| **1. Problem Description & Background** | Clear problem statement, context, and motivation | **15** |
| **2. Algorithm Explanation** | Clear description, correct reasoning, and step-by-step clarity | **30** |
| **3. Complexity Analysis** | Correct time & space complexity analysis | **15** |
| **4. Evaluation & Results** | Test cases and performance analysis | **10** |
| **5. Comparative Discussion** | Comparison with alternative algorithms/approaches | **10** |
| **6. Report Organization & Writing** | Structure, clarity, and technical writing | **10** |
| **7. GitHub Repository**
**8. References & Academic Integrity** | Proper organization and a readme file
Proper citation and originality | **5**
**5** |

## On Grading

The right is reserved to use neither rubric, and score the assignment as appropriate. The rubric isn't "settled" and I do not plan to publish the final rubric used. Your grade will be relative to the work submitted by other teams. If you want the best score, submit the best work.

The right is reserved to use neither rubric, and score the assignment as appropriate. The rubric isn't "settled" and I do not plan to publish the final rubric used. Your grade will be relative to the work submitted by other teams. If you want the best score, submit the best work. (My usual grading method: any *big* deficiency is a 10 point deduction -- more than 100 points of deductions may be possible but I'll stop at zero. Small deficiency is a -5 deduction. Admittedly my method has some gray areas).

## Final Thought

Better asymptotic bounds do not always imply better real-world performance.

### Notes

- Testing should be included in submission. Graders should be able to look and see your results without compiling and running your code. you should have a testing framework and the results of your test cases in repo.
- No screenshots
- Don’t use python. Better to use C, C++
- You can use the vector library but should write most of it yourself