[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 1
  nz = 10
  zmax = 0
  zmin = -1.0
[]

[Modules]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
    [../]
  [../]
[]

[UserObjects]
  [./rock_uo]
    type =  TigerPermeabilityConst
    permeability_type = orthotropic
    k0 = '3.0e-10 2.0e-10 1.0e-10'
  [../]
[]

[Materials]
  [./rock_g]
    type = TigerGeometryMaterial
    gravity = '0 0 -9.81 '
  [../]
  [./rock_p]
    type = TigerPorosityMaterial
    porosity = 0.4
    specific_density = 2500
  [../]
  [./rock_f]
    type = TigerFluidMaterial
    fp_uo = water_uo
  [../]
  [./rock_h]
    type = TigerHydraulicMaterialH
    pressure = pressure
    compressibility = 1.0e-9
    kf_uo = rock_uo
  [../]
[]

[BCs]
  [./front]
    type =  DirichletBC
    variable = pressure
    boundary = 'front'
    value = 0
  [../]
[]

[AuxVariables]
  [./vx]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./vy]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./vz]
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[AuxKernels]
  [./vx_ker]
    type = TigerDarcyVelocityH
    pressure = pressure
    variable =  vx
    component = x
  [../]
  [./vy_ker]
    type = TigerDarcyVelocityH
    pressure = pressure
    variable =  vy
    component = y
  [../]
  [./vz_ker]
    type = TigerDarcyVelocityH
    pressure = pressure
    variable =  vz
    component = z
  [../]
[]

[Variables]
  [./pressure]
    scaling = 1e5
  [../]
[]

[Kernels]
  [./diff]
    type = TigerHydraulicKernelH
    variable = pressure
  [../]
  [./time]
    type = TigerHydraulicTimeKernelH
    variable = pressure
  [../]
[]

[Executioner]
  type = Transient
  dt = 0.01
  end_time = 0.3
  nl_rel_tol = 1e-08
  nl_rel_step_tol = 1e-14 #machine percision
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]
