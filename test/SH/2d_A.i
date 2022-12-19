[Mesh]
  type = GeneratedMesh
  dim = 2
  xmin = -1
  xmax = 1
  ymin = -1
  ymax = 1
  nx = 20
  ny = 20
[]

[Modules]
  [./FluidProperties]
    [./water_uo]
      type = TigerWaterConst
    [../]
  [../]
[]

[UserObjects]
  [./supg]
    type = TigerSUPG
    effective_length = min
    supg_coeficient = transient_tezduyar
  [../]
[]

[Materials]
  [./rock_f]
    type = TigerFluidMaterial
    fp_uo = water_uo
  [../]
  [./rock_g]
    type = TigerGeometryMaterial
  [../]
  [./rock_p]
    type = TigerPorosityMaterial
    porosity = 0.5
    specific_density = 2500
  [../]
  [./advect_sh]
    type = TigerSoluteMaterialS
    advection_type = user_velocity
    user_velocity = vel
    has_supg = true
    supg_uo = supg
    diffusion = 0
  [../]
[]

[BCs]
  [./whole_s]
    type = DirichletBC
    variable = solute
    boundary = 'left right top bottom'
    value = 0
  [../]
[]

[Functions]
  [./vel]
    type = ParsedVectorFunction
    value_x = '-sqrt(x*x+y*y)*sin(atan(y/x))'
    value_y = 'sqrt(x*x+y*y)*cos(atan(y/x))'
  [../]
  [./sol]
    type = ParsedFunction
    value = 'if(sqrt((x+0.5)*(x+0.5)+y*y)<0.25,0.5*(cos(4*3.14*sqrt((x+0.5)*(x+0.5)+y*y))+1),0.0)'
  [../]
[]

[ICs]
  [./init_s]
    type = FunctionIC
    function = sol
    variable = solute
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
  [./pe]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./cr]
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[AuxKernels]
  [./vx_ker]
    type = MaterialRealVectorValueAux
    property = 'solute_advection_velocity'
    variable = vx
    component = 0
  [../]
  [./vy_ker]
    type = MaterialRealVectorValueAux
    property = 'solute_advection_velocity'
    variable = vy
    component = 1
  [../]
  [./pe_ker]
    type = MaterialRealAux
    property = 'solute_peclet_number'
    variable = pe
  [../]
  [./cr_ker]
    type = MaterialRealAux
    property = 'solute_courant_number'
    variable = cr
  [../]
[]

[Variables]
  [./solute]
  [../]
[]

[Kernels]
  [./S_advect]
    type = TigerSoluteAdvectionKernelS
    variable = solute
  [../]
  [./S_dt]
    type = TigerSoluteTimeKernelS
    variable = solute
  [../]
[]

[Executioner]
  type = Transient
  dt = 3.14e-2
  end_time = 3.14
  solve_type = NEWTON
  [./TimeIntegrator]
     type = CrankNicolson
  [../]
  petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it -ksp_max_it -sub_pc_type -sub_pc_factor_shift_type'
  petsc_options_value = 'gmres asm 1E-10 1E-10 200 500 lu NONZERO'
[]

[Outputs]
  exodus = true
  interval = 10
[]
