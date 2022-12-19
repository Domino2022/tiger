[Mesh]
  type = FileMesh
  file = 3d_curvedfrac.msh
[]

[Modules]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
    [../]
  [../]
[]

[GlobalParams]
  conductivity_type = isotropic
  density = 2600
  specific_heat = 840
[]

[Materials]
  [./rock_f]
    type = TigerFluidMaterial
    fp_uo = water_uo
  [../]
  [./rock_g1]
    type = TigerGeometryMaterial
    block = 'b1 b2'
  [../]
  [./rock_g2]
    type = TigerGeometryMaterial
    scale_factor = 0.1
    block = frac
  [../]
  [./rock_p1]
    type = TigerPorosityMaterial
    porosity = 0
    specific_density = 0
    block = 'b1 b2'
  [../]
  [./rock_p2]
    type = TigerPorosityMaterial
    porosity = 0
    specific_density = 0
    block = frac
  [../]
  [./rock_t]
    type = TigerThermalMaterialT
    advection_type = pure_diffusion
    lambda = 2
  [../]
[]

[BCs]
  [./front]
    type =  DirichletBC
    variable = temperature
    boundary = top
    value = 0
  [../]
  [./back]
    type =  DirichletBC
    variable = temperature
    boundary = bottom
    value = 100.0
  [../]
[]

[Variables]
  [./temperature]
    initial_condition = 0.0
  [../]
[]

[Kernels]
  [./T_diff]
    type = TigerThermalDiffusionKernelT
    variable = temperature
  [../]
  [./T_dt]
    type = TigerThermalTimeKernelT
    variable = temperature
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 10
  end_time = 1.0e5
  l_tol = 1e-10 #difference between first and last linear step
  nl_rel_step_tol = 1e-14 #machine percision
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
  print_linear_residuals = false
[]
