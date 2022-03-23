
export interface TrackingInfo {
    // Id of the task being completed
    taskId: string;
    // Id of the participant currently completing the task
    participantId: string;
}

export interface QueryEvent {
    // The the query happened
    participantId: string;
    taskId: string;
    positiveExamples: string[];
    negativeExamples: string[];
}
