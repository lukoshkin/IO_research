#include <dolfin.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "P1.h"

#define N_EL 32

using namespace dolfin;
using array_2d = std::vector<std::vector<double>>;


array_2d
build_mass_matrix(array_2d& F, std::vector<double>& K) {
    auto mesh = std::make_shared<UnitSquareMesh>(N_EL, N_EL);
    auto V = std::make_shared<P1::Form_M_FunctionSpace_0>(mesh);

    auto u = std::make_shared<Function>(V);
    auto v = std::make_shared<Function>(V);
    auto k = std::make_shared<Function>(V);
    k->vector()->set_local(K);

    P1::Form_M mass_form(mesh, k, u, v);
    array_2d mass(4*N_EL, std::vector<double>(4*N_EL));

    for (int i=0; i<4*N_EL; ++i) {
        u->vector()->set_local(F[i]);
        for (int j=i; j<4*N_EL; ++j) {
            v->vector()->set_local(F[j]);
            mass[i][j] = assemble(mass_form);
        }
    }
    return mass;
}


array_2d
build_stiffness_matrix(array_2d& F, std::vector<double>& K) {
    auto mesh = std::make_shared<UnitSquareMesh>(N_EL, N_EL);
    auto V = std::make_shared<P1::Form_S_FunctionSpace_0>(mesh);

    auto u = std::make_shared<Function>(V);
    auto v = std::make_shared<Function>(V);
    auto k = std::make_shared<Function>(V);
    k->vector()->set_local(K);

    P1::Form_S stiffness_form(mesh, k, u, v);
    array_2d stiffness(4*N_EL, std::vector<double>(4*N_EL));

    for (int i=0; i<4*N_EL; ++i) {
        u->vector()->set_local(F[i]);
        for (int j=i; j<4*N_EL; ++j) {
            v->vector()->set_local(F[j]);
            stiffness[i][j] = assemble(stiffness_form);
        }
    }
    return stiffness;
}

PYBIND11_MODULE(SIGNATURE, m) {
    m.doc() = "A faster construction of mass and stiffness forms with C++";
    m.def(
            "build_mass_matrix",
            &build_mass_matrix, 
            "<K\\grad(u_i), \\grad(u_j)>");

    m.def(
            "build_stiffness_matrix",
            &build_stiffness_matrix,
            "<K u_i, u_j>");
}
