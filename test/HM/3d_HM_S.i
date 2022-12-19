[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 10
  nz = 1
  ymax = 0
  ymin = -1
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Modules]
  [./TensorMechanics]
    [./Master]
      [./all]
      add_variables = true
      strain = SMALL
      generate_output = 'stress_xx stress_zz stress_yy vonmises_stress'
      [../]
    [../]
  [../]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
    [../]
  [../]
[]

[UserObjects]
  [./rock_uo]
    type =  TigerPermeabilityConst
    permeability_type = isotropic
    k0 = '1.0e-10'
  [../]
[]

[Variables]
  [./pressure]
  [../]
[]

[Kernels]
  [./gravity_y]
    type = TigerMechanicsGravityM
    variable = disp_y
    component = 1
  [../]
  [./diff]
    type = TigerHydraulicKernelH
    variable = pressure
  [../]
  [./poro_x]
    type = PoroMechanicsCoupling
    variable = disp_x
    porepressure = pressure
    component = 0
  [../]
  [./poro_y]
    type = PoroMechanicsCoupling
    variable = disp_y
    porepressure = pressure
    component = 1
  [../]
  [./poro_z]
    type = PoroMechanicsCoupling
    variable = disp_z
    porepressure = pressure
    component = 2
  [../]
[]

[BCs]
  [./no_x]
    type = DirichletBC
    variable = disp_x
    boundary = bottom
    value = 0.0
  [../]
  [./no_y]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0.0
  [../]
  [./no_z]
    type = DirichletBC
    variable = disp_z
    boundary = bottom
    value = 0.0
  [../]
  [./pressure]
    type =  DirichletBC
    variable = pressure
    boundary = top
    value = 0
  [../]
[]

[Materials]
  [./Elasticity_tensor]
    type = ComputeElasticityTensor
    fill_method = symmetric_isotropic
    C_ijkl = '0 0.5e8'
  [../]
  [./stress]
    type = ComputeLinearElasticStress
  [../]
  [./rock_g]
    type = TigerGeometryMaterial
    gravity = '0 -9.8 0'
  [../]
  [./rock_p]
    type = TigerPorosityMaterial
    porosity = 0.2
    specific_density = 2500
  [../]
  [./rock_m]
    type = TigerMechanicsMaterialM
    disps = 'disp_x disp_y disp_z'
    incremental = false
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

[Executioner]
  type = Steady
  solve_type = PJFNK
  nl_abs_tol = 1e-10
  l_max_its = 20
[]

[Outputs]
  [./out]
    type = Exodus
    elemental_as_nodal = true
  [../]
[]
