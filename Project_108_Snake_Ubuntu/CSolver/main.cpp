/*`
 *   Copyright 2017 Toby Liu
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

#include "rubik_cube_solver.hpp"

#include <string>
#include <iostream>

int main() {
    Rubik_Cube rb(3);

    rb.Dump();

    std::cout << "Scramble cube:" << std::endl;
    // std::string moves = rb.Scramble();
    std::string moves = "R U' L' U R' U' L U";
    // moves = "R L" ;

    rb.Move(moves);    

    std::cout << "Moves Scramble: " << moves << std::endl;
    rb.Dump();

    // std::cout << "Inverse scramble steps:" << std::endl;
    // rb.Inverse(moves);
    // rb.Dump();

    // std::cout << "Scramble cube again:" << std::endl;
    // rb.Scramble();
    // rb.Dump();
    Rubik_CubeSolver *solver = new Rubik_Cube3BasicSolver(rb);
    moves = solver->Solve();

    std::cout << "Moves solved by basic solver: " << moves << std::endl;
    rb.Move(moves);    
    rb.Dump();

    return 0;
}