#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constantes
#define MAX_PACIENTES 1000
#define MAX_SALAS 100
#define MAX_ESPECIALIDADES 50
#define MAX_MEDICOS 200
#define MAX_CONSULTAS 5000

// Estruturas
typedef struct {
    int id;
    char nome[50];
    int idade;
    float altura;
    float peso;
    char sintomas[100];
    int prioridade;
} Paciente;

typedef struct {
    int id;
    char nome[50];
    int especialidadeId;
    int horasTrabalhadas;
} Medico;

typedef struct {
    int id;
    char nome[50];
} Sala;

typedef struct {
    int pacienteId; 
    int medicoId;   
    int salaId;
    int horario; 
    int retorno;
} Consulta;


// Função para ler dados
void lerDados(Paciente *pacientes, int *numPacientes, Medico *medicos, int *numMedicos, Sala *salas, int *numSalas) {
    FILE *arquivo = fopen("entrada.txt", "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        return;
    }

    char linha[256];
    
    // Lendo pacientes
    fgets(linha, sizeof(linha), arquivo);
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strncmp(linha, "Salas:", 6) == 0) break; // Próxima seção
        if (*numPacientes >= MAX_PACIENTES) {
            printf("Número máximo de pacientes excedido.\n");
            break;
        }
        sscanf(linha, "%d %s %d %f %f %s %d", 
               &pacientes[*numPacientes].id,
               pacientes[*numPacientes].nome,
               &pacientes[*numPacientes].idade,
               &pacientes[*numPacientes].altura,
               &pacientes[*numPacientes].peso,
               pacientes[*numPacientes].sintomas,
               &pacientes[*numPacientes].prioridade);
        (*numPacientes)++;
    }

    // Lendo salas
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strncmp(linha, "Especialidades:", 15) == 0) break; // Próxima seção
        if (*numSalas >= MAX_SALAS) {
            printf("Número máximo de salas excedido.\n");
            break;
        }
        sscanf(linha, "%d %s", 
               &salas[*numSalas].id,
               salas[*numSalas].nome);
        (*numSalas)++;
    }

    // Lendo especialidades
    int numEspecialidades = 0;
    char especialidades[MAX_ESPECIALIDADES][50];
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strncmp(linha, "Médicos:", 8) == 0) break; // Próxima seção
        if (numEspecialidades >= MAX_ESPECIALIDADES) {
            printf("Número máximo de especialidades excedido.\n");
            break;
        }
        sscanf(linha, "%d %s", 
               &numEspecialidades, 
               especialidades[numEspecialidades]);
        numEspecialidades++;
    }

    // Lendo médicos
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (*numMedicos >= MAX_MEDICOS) {
            printf("Número máximo de médicos excedido.\n");
            break;
        }
        sscanf(linha, "%d %s %d", &medicos[(*numMedicos)].id, medicos[(*numMedicos)].nome, &medicos[(*numMedicos)].especialidadeId);

        medicos[*numMedicos].horasTrabalhadas = 0; // Inicializa horas trabalhadas
        (*numMedicos)++;
    }

    fclose(arquivo);
    printf("Dados lidos do arquivo com sucesso!\n");
    printf("Pacientes lidos: %d\n", *numPacientes);
    printf("Salas lidas: %d\n", *numSalas);
    printf("Especialidades lidas: %d\n", numEspecialidades);
    printf("Médicos lidos: %d\n", *numMedicos);
}

// Função para alocar consultas
void alocarConsultas(Paciente *pacientes, int numPacientes, Medico *medicos, int numMedicos, 
                     Sala *salas, int numSalas, Consulta *consultas, int *numConsultas) {
    int horarioAtual = 8; // Início do expediente
    for (int i = 0; i < numPacientes; i++) {
        if (*numConsultas >= MAX_CONSULTAS) break;

        // Aloca médico e sala disponíveis
        for (int m = 0; m < numMedicos; m++) {
            for (int s = 0; s < numSalas; s++) {
                if (horarioAtual < 17) { // Dentro do horário comercial
                    Consulta novaConsulta = {
                        .pacienteId = pacientes[i].id,
                        .medicoId = medicos[m].id,
                        .salaId = salas[s].id,
                        .horario = horarioAtual,
                        .retorno = 0
                    };
                    consultas[(*numConsultas)++] = novaConsulta;

                    medicos[m].horasTrabalhadas++;
                    (*numConsultas)++;
                    horarioAtual++;
                    break;
                }
            }
        }
    }
}

// Função para gerar relatório
void gerarRelatorio(Consulta *consultas, int numConsultas, Medico *medicos, int numMedicos) {
    FILE *arquivo = fopen("relatorio.txt", "w");
    if (!arquivo) {
        printf("Erro ao abrir arquivo para salvar o relatório.\n");
        return;
    }

    fprintf(arquivo, "=== Relatório de Consultas ===\n\n");
    fprintf(arquivo, "Consultas Realizadas:\n");
    for (int i = 0; i < numConsultas; i++) {
        fprintf(arquivo, "Consulta %d: Paciente %d, Médico %d, Sala %d, Horário %d, Retorno %d\n",
            i + 1,
            consultas[i].pacienteId,
            consultas[i].medicoId,
            consultas[i].salaId,
            consultas[i].horario,
            consultas[i].retorno
        );
    }

    fprintf(arquivo, "\nResumo de Horas Trabalhadas:\n");
    for (int i = 0; i < numMedicos; i++) {
        fprintf(arquivo, "- Médico %s (ID %d): %d %s\n",
                medicos[i].nome, medicos[i].id, medicos[i].horasTrabalhadas,
                medicos[i].horasTrabalhadas == 1 ? "hora" : "horas");
    }

    fclose(arquivo);
    printf("Relatório salvo em 'relatorio.txt'.\n");
}

void gerenciarRetornos(Consulta *consultas, int *numConsultas, int maxConsultas) {
    for (int i = 0; i < *numConsultas; i++) {
        if (consultas[i].retorno == 1 && *numConsultas < maxConsultas) {
            Consulta novaConsulta = consultas[i];
            novaConsulta.horario += 1; // Ajuste o horário de retorno
            novaConsulta.retorno = 0; // Retornos não têm direito a novos retornos
            consultas[*numConsultas] = novaConsulta;
            (*numConsultas)++;
        }
    }
}

// Função principal
int main() {
    Paciente pacientes[MAX_PACIENTES];
    Medico medicos[MAX_MEDICOS];
    Sala salas[MAX_SALAS];
    Consulta consultas[MAX_CONSULTAS];
    int numPacientes = 0, numMedicos = 0, numSalas = 0, numConsultas = 0;

    // Ler dados do arquivo
    lerDados(pacientes, &numPacientes, medicos, &numMedicos, salas, &numSalas);

    // Alocar consultas
    alocarConsultas(pacientes, numPacientes, medicos, numMedicos, salas, numSalas, consultas, &numConsultas);

    // Gerenciar retornos
    gerenciarRetornos(consultas, &numConsultas, MAX_CONSULTAS);

    // Gerar relatório
    gerarRelatorio(consultas, numConsultas, medicos, numMedicos);

    return 0;
}
