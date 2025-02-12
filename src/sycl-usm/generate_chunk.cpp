/*
 Crown Copyright 2012 AWE.

 This file is part of CloverLeaf.

 CloverLeaf is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the
 Free Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 CloverLeaf is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 CloverLeaf. If not, see http://www.gnu.org/licenses/.
 */

//  @brief Mesh chunk generation driver
//  @author Wayne Gaudin
//  @details Invoked the users specified chunk generator.
//  @brief Mesh chunk generation driver
//  @author Wayne Gaudin
//  @details Invoked the users specified chunk generator.

#include "generate_chunk.h"
#include "context.h"

void generate_chunk(const int tile, global_variables &globals) {

  // Need to copy the host array of state input data into a device array
  clover::Buffer1D<double> state_density(globals.context, globals.config.number_of_states);
  clover::Buffer1D<double> state_energy(globals.context, globals.config.number_of_states);
  clover::Buffer1D<double> state_xvel(globals.context, globals.config.number_of_states);
  clover::Buffer1D<double> state_yvel(globals.context, globals.config.number_of_states);
  clover::Buffer1D<double> state_xmin(globals.context, globals.config.number_of_states);
  clover::Buffer1D<double> state_xmax(globals.context, globals.config.number_of_states);
  clover::Buffer1D<double> state_ymin(globals.context, globals.config.number_of_states);
  clover::Buffer1D<double> state_ymax(globals.context, globals.config.number_of_states);
  clover::Buffer1D<double> state_radius(globals.context, globals.config.number_of_states);
  clover::Buffer1D<int> state_geometry(globals.context, globals.config.number_of_states);

  // Copy the data to the new views
  for (int state = 0; state < globals.config.number_of_states; ++state) {
    state_density[state] = globals.config.states[state].density;
    state_energy[state] = globals.config.states[state].energy;
    state_xvel[state] = globals.config.states[state].xvel;
    state_yvel[state] = globals.config.states[state].yvel;
    state_xmin[state] = globals.config.states[state].xmin;
    state_xmax[state] = globals.config.states[state].xmax;
    state_ymin[state] = globals.config.states[state].ymin;
    state_ymax[state] = globals.config.states[state].ymax;
    state_radius[state] = globals.config.states[state].radius;
    state_geometry[state] = globals.config.states[state].geometry;
  }

  // Kokkos::deep_copy (TO, FROM)

  const int x_min = globals.chunk.tiles[tile].info.t_xmin;
  const int x_max = globals.chunk.tiles[tile].info.t_xmax;
  const int y_min = globals.chunk.tiles[tile].info.t_ymin;
  const int y_max = globals.chunk.tiles[tile].info.t_ymax;

  size_t xrange = (x_max + 2) - (x_min - 2) + 1;
  size_t yrange = (y_max + 2) - (y_min - 2) + 1;

  // Take a reference to the lowest structure, as Kokkos device cannot necessarily chase through the structure.

  clover::Range2d xyrange_policy(0u, 0u, xrange, yrange);

  auto &field = globals.chunk.tiles[tile].field;

  // State 1 is always the background state

  clover::par_ranged2(globals.context.queue, xyrange_policy, [=](const int i, const int j) {
    field.energy0(i, j) = state_energy[0];
    field.density0(i, j) = state_density[0];
    field.xvel0(i, j) = state_xvel[0];
    field.yvel0(i, j) = state_yvel[0];
  });

  for (int state = 1; state < globals.config.number_of_states; ++state) {

    clover::par_ranged2(globals.context.queue, xyrange_policy, [=](const int x, const int y) {
      const int j = x;
      const int k = y;

      double x_cent = state_xmin[state];
      double y_cent = state_ymin[state];

      if (state_geometry[state] == g_rect) {
        if (field.vertexx[j + 1] >= state_xmin[state] && field.vertexx[j] < state_xmax[state]) {
          if (field.vertexy[k + 1] >= state_ymin[state] && field.vertexy[k] < state_ymax[state]) {
            field.energy0(x, y) = state_energy[state];
            field.density0(x, y) = state_density[state];
            for (int kt = k; kt <= k + 1; ++kt) {
              for (int jt = j; jt <= j + 1; ++jt) {
                field.xvel0(jt, kt) = state_xvel[state];
                field.yvel0(jt, kt) = state_yvel[state];
              }
            }
          }
        }
      } else if (state_geometry[state] == g_circ) {
        double radius =
            sycl::sqrt((field.cellx[j] - x_cent) * (field.cellx[j] - x_cent) + (field.celly[k] - y_cent) * (field.celly[k] - y_cent));
        if (radius <= state_radius[state]) {
          field.energy0(x, y) = state_energy[state];
          field.density0(x, y) = state_density[state];
          for (int kt = k; kt <= k + 1; ++kt) {
            for (int jt = j; jt <= j + 1; ++jt) {
              field.xvel0(jt, kt) = state_xvel[state];
              field.yvel0(jt, kt) = state_yvel[state];
            }
          }
        }
      } else if (state_geometry[state] == g_point) {
        if (field.vertexx[j] == x_cent && field.vertexy[k] == y_cent) {
          field.energy0(x, y) = state_energy[state];
          field.density0(x, y) = state_density[state];
          for (int kt = k; kt <= k + 1; ++kt) {
            for (int jt = j; jt <= j + 1; ++jt) {
              field.xvel0(jt, kt) = state_xvel[state];
              field.yvel0(jt, kt) = state_yvel[state];
            }
          }
        }
      }
    });
  }

  clover::free(globals.context.queue,                          //
               state_density, state_energy,                    //
               state_xvel, state_yvel,                         //
               state_xmin, state_xmax, state_ymin, state_ymax, //
               state_radius, state_geometry);
}
