def Header(f, filename):
    header_template = (
        'apiVersion: argoproj.io/v1alpha1\n'
        'kind: Workflow\n'
        'metadata:\n'
        '  generateName: {}-\n'.format(filename.split('.')[0]) +
        'spec:\n'
        '  entrypoint: main\n'
        '  volumes:\n'
        '  - name: shared-data\n'
        '    hostPath:\n'
        '      path: /home/share/\n'
        '  templates:\n'
    )
    f.write(header_template)


def Executor(f, name, container_command):
    template = f'  - name: {name}\n'
    template += (
        '    container:\n'
        f'      image: argoSimulationModel:latest\n'
        '      command: ["sh", "-c"]\n'
        f'      args: ["{container_command}"]\n'
        '      volumeMounts:\n'
        '      - name: shared-data\n'
        '        mountPath: /etc/\n'
    )
    f.write(template + '\n')


def StepGenerator(f, num_layers, max_sim):
    main_template = '  - name: main\n'
    main_template += '    dag:\n'
    main_template += '      tasks:\n'
    for layer in range(num_layers + 1):
        if layer == 0:
            main_template += f'      - name: execute-argooptexec-{layer}\n'
            main_template += f'        template: argooptexec-{layer}\n'
            for sim in range(max_sim):
                main_template += f'      - name: execute-simrunner-{layer}-{sim}\n'
                main_template += f'        template: simrunner-{layer}-{sim}\n'
                main_template += f'        dependencies: ["execute-argooptexec-{layer}"]\n'
        else:
            dependencies = ', '.join(f'"execute-simrunner-{layer-1}-{sim}"' for sim in range(max_sim))
            main_template += f'      - name: execute-argooptexec-{layer}\n'
            main_template += f'        template: argooptexec-{layer}\n'
            main_template += f'        dependencies: [{dependencies}]\n'
            if(layer < num_layers):
                for sim in range(max_sim):
                    main_template += f'      - name: execute-simrunner-{layer}-{sim}\n'
                    main_template += f'        template: simrunner-{layer}-{sim}\n'
                    main_template += f'        dependencies: ["execute-argooptexec-{layer}"]\n'
    f.write(main_template + '\n')




def generate_yaml_file(filename, num_layers, num_sim):
    with open("/root/"+filename, 'w') as f:
        add_header(f, filename)
        add_workflow_structure(f, num_layers, num_sim)
        for layer in range(num_layers+1):  # Increase the range by 1
            add_workflow_execution(f, f'argooptexec-{layer}', f'/app/bin/ArgoOptExec {0 if(layer == 0) else 1}')
            if layer < num_layers:  # Only add simrunner templates for the original layers
                for sim in range(num_sim):
                    add_workflow_execution(f, f'simrunner-{layer}-{sim}', f'/app/bin/SimRunner {sim}')


generate_yaml_file("automatic-test.yaml", 4, 2)