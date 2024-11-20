#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char nome[50];
    int idade;
    float peso;
    float altura;
    char sintomas[100];
    char medicacao[50];
    char telefone[15];
    int prioridade; // Gerado aleatoriamente
    int especialidade_id;
    int faltou; // 0 = não faltou, 1 = faltou
} Paciente;

typedef struct {
    int id;
    char nome[50];
} Especialidade;

typedef struct {
    int id;
    char nome[50];
    int especialidade_id;
} Medico;

typedef struct {
    int id;
    char nome[50];
} Sala;

typedef struct {
    int paciente_id;
    int medico_id;
    int sala_id;
    int horario; // Hora do dia (0 = 8:00, 1 = 9:00, etc.)
    int dia;     // Dia da semana (0 = Segunda, 1 = Terça, etc.)
} Consulta;


void lerDados(char *nomeArquivo, Paciente *pacientes, Sala *salas, Especialidade *especialidades, Medico *medicos,
              int *totalPacientes, int *totalSalas, int *totalEspecialidades, int *totalMedicos) {
    FILE *file = fopen(nomeArquivo, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo de entrada!\n");
        exit(1);
    }

    // Leitura de pacientes
    fscanf(file, "%d", totalPacientes);
    for (int i = 0; i < *totalPacientes; i++) {
        fscanf(file, "%d %s %d %f %f %s %s %s %d %d",
               &pacientes[i].id, pacientes[i].nome, &pacientes[i].idade,
               &pacientes[i].peso, &pacientes[i].altura, pacientes[i].sintomas,
               pacientes[i].medicacao, pacientes[i].telefone, &pacientes[i].prioridade,
               &pacientes[i].especialidade_id);
        pacientes[i].faltou = 0; // Inicializa como "não faltou"
    }

    // Leitura de salas
    fscanf(file, "%d", totalSalas);
    for (int i = 0; i < *totalSalas; i++) {
        fscanf(file, "%d %s", &salas[i].id, salas[i].nome);
    }

    // Leitura de especialidades
    fscanf(file, "%d", totalEspecialidades);
    for (int i = 0; i < *totalEspecialidades; i++) {
        fscanf(file, "%d %s", &especialidades[i].id, especialidades[i].nome);
    }

    // Leitura de médicos
    fscanf(file, "%d", totalMedicos);
    for (int i = 0; i < *totalMedicos; i++) {
        fscanf(file, "%d %s %d", &medicos[i].id, medicos[i].nome, &medicos[i].especialidade_id);
    }

    fclose(file);
}

void alocarConsultas(Paciente *pacientes, Medico *medicos, Sala *salas, Consulta *consultas,
                     int totalPacientes, int totalMedicos, int totalSalas, int *totalConsultas) {
    int horarioAtual = 0; // Começa no horário 8:00
    int diaAtual = 0;     // Começa na segunda-feira
    *totalConsultas = 0;

    for (int i = 0; i < totalPacientes; i++) {
        if (pacientes[i].faltou) continue; // Ignorar pacientes que faltaram

        // Encontra um médico disponível com a mesma especialidade
        for (int j = 0; j < totalMedicos; j++) {
            if (medicos[j].especialidade_id == pacientes[i].especialidade_id) {
                // Encontra uma sala disponível
                for (int k = 0; k < totalSalas; k++) {
                    consultas[*totalConsultas].paciente_id = pacientes[i].id;
                    consultas[*totalConsultas].medico_id = medicos[j].id;
                    consultas[*totalConsultas].sala_id = salas[k].id;
                    consultas[*totalConsultas].horario = horarioAtual;
                    consultas[*totalConsultas].dia = diaAtual;
                    (*totalConsultas)++;

                    // Atualiza horário e dia
                    horarioAtual++;
                    if (horarioAtual == 8) { // Assume 8 horas de trabalho por dia
                        horarioAtual = 0;
                        diaAtual++;
                    }
                    if (diaAtual == 7) diaAtual = 0; // Semana seguinte
                    break;
                }
                break;
            }
        }
    }
}

void gerenciarRetornos(Paciente *pacientes, Consulta *consultas, int totalPacientes, int totalConsultas) {
    for (int i = 0; i < totalConsultas; i++) {
        int pacienteId = consultas[i].paciente_id;
        if (pacientes[pacienteId].faltou == 1) {
            pacientes[pacienteId].prioridade--; // Reduz prioridade para faltosos
        } else {
            // Verifica se o paciente precisa de retorno em até 30 dias
            pacientes[pacienteId].prioridade++; // Aumenta prioridade para retornos
        }
    }
}

void gerarRelatorio(Consulta *consultas, Medico *medicos, int totalConsultas, int totalMedicos) {
    FILE *file = fopen("relatorio.txt", "w");
    if (!file) {
        printf("Erro ao criar o relatório!\n");
        exit(1);
    }

    // Relatório de alocação
    fprintf(file, "Alocação de Consultas:\n");
    for (int i = 0; i < totalConsultas; i++) {
        fprintf(file, "Consulta %d: Paciente %d, Médico %d, Sala %d, Horário %d:00, Dia %d\n",
                i + 1, consultas[i].paciente_id, consultas[i].medico_id,
                consultas[i].sala_id, consultas[i].horario + 8, consultas[i].dia + 1);
    }

    // Relatório de horas trabalhadas por médico
    fprintf(file, "\nHoras Trabalhadas por Médico:\n");
    int horasTrabalhadas[totalMedicos];
    memset(horasTrabalhadas, 0, sizeof(horasTrabalhadas));
    for (int i = 0; i < totalConsultas; i++) {
        horasTrabalhadas[consultas[i].medico_id]++;
    }
    for (int i = 0; i < totalMedicos; i++) {
        fprintf(file, "Médico %d (%s): %d horas\n", medicos[i].id, medicos[i].nome, horasTrabalhadas[i]);
    }

    fclose(file);
}



int main() {
    Paciente pacientes[100];
    Sala salas[10];
    Especialidade especialidades[10];
    Medico medicos[10];
    Consulta consultas[500];

    // Leitura dos dados de entrada
    lerDados("entrada.txt", pacientes, salas, especialidades, medicos);

    // Alocação de consultas
    alocarConsultas(pacientes, medicos, salas, consultas);

    // Gerenciamento de retornos e faltas
    gerenciarRetornos(pacientes, consultas);

    // Geração de relatórios
    gerarRelatorio(consultas, medicos);

    return 0;
}

